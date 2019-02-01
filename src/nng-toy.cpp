#include <cstdio>
#include <cstdlib>
#include <string>
#include <thread>
#include <vector>

#include <nng/nng.h>
#include <nng/protocol/pipeline0/pull.h>
#include <nng/protocol/pipeline0/push.h>
#include <nng/protocol/reqrep0/rep.h>
#include <nng/protocol/reqrep0/req.h>

void fatal(const char *func, int rv) {
    std::fprintf(stderr, "%s: %s\n", func, nng_strerror(rv));
    std::exit(1);
}

#define DATECMD 1
#define CLIENT "client"
#define SERVER "server"

#define PUT64(ptr, u)                                                                              \
    do {                                                                                           \
        (ptr)[0] = (uint8_t)(((uint64_t)(u)) >> 56);                                               \
        (ptr)[1] = (uint8_t)(((uint64_t)(u)) >> 48);                                               \
        (ptr)[2] = (uint8_t)(((uint64_t)(u)) >> 40);                                               \
        (ptr)[3] = (uint8_t)(((uint64_t)(u)) >> 32);                                               \
        (ptr)[4] = (uint8_t)(((uint64_t)(u)) >> 24);                                               \
        (ptr)[5] = (uint8_t)(((uint64_t)(u)) >> 16);                                               \
        (ptr)[6] = (uint8_t)(((uint64_t)(u)) >> 8);                                                \
        (ptr)[7] = (uint8_t)((uint64_t)(u));                                                       \
    } while (0)

#define GET64(ptr, v)                                                                              \
    v = (((uint64_t)((uint8_t)(ptr)[0])) << 56) + (((uint64_t)((uint8_t)(ptr)[1])) << 48) +        \
        (((uint64_t)((uint8_t)(ptr)[2])) << 40) + (((uint64_t)((uint8_t)(ptr)[3])) << 32) +        \
        (((uint64_t)((uint8_t)(ptr)[4])) << 24) + (((uint64_t)((uint8_t)(ptr)[5])) << 16) +        \
        (((uint64_t)((uint8_t)(ptr)[6])) << 8) + (((uint64_t)(uint8_t)(ptr)[7]))

void showdate(time_t now) {
    struct tm *info = localtime(&now);
    printf("%s", asctime(info));
}

int server(const char *url) {
    nng_socket sock;
    int rv;

    if ((rv = nng_rep0_open(&sock)) != 0) {
        fatal("nng_rep0_open", rv);
    }
    if ((rv = nng_listen(sock, url, NULL, 0)) != 0) {
        fatal("nng_listen", rv);
    }
    for (;;) {
        char *buf = NULL;
        size_t sz;
        uint64_t val;
        if ((rv = nng_recv(sock, &buf, &sz, NNG_FLAG_ALLOC)) != 0) {
            fatal("nng_recv", rv);
        }
        if ((sz == sizeof(uint64_t)) && ((GET64(buf, val)) == DATECMD)) {
            time_t now;
            printf("SERVER: RECEIVED DATE REQUEST\n");
            now = time(&now);
            printf("SERVER: SENDING DATE: ");
            showdate(now);

            // Reuse the buffer.  We know it is big enough.
            PUT64(buf, (uint64_t)now);
            rv = nng_send(sock, buf, sz, NNG_FLAG_ALLOC);
            if (rv != 0) {
                fatal("nng_send", rv);
            }
            continue;
        }
        // Unrecognized command, so toss the buffer.
        nng_free(buf, sz);
    }
}

int client(const char *url) {
    nng_socket sock;
    char *buf = NULL;
    size_t sz;
    uint8_t cmd[sizeof(uint64_t)];
    int rv;

    PUT64(cmd, DATECMD);

    if ((rv = nng_req0_open(&sock)) != 0) {
        fatal("nng_socket", rv);
    }
    if ((rv = nng_dial(sock, url, NULL, 0)) != 0) {
        fatal("nng_dial", rv);
    }
    std::printf("CLIENT: REQUEST\n");
    if ((rv = nng_send(sock, cmd, sizeof(cmd), 0)) != 0) {
        fatal("nng_send", rv);
    }
    if ((rv = nng_recv(sock, &buf, &sz, NNG_FLAG_ALLOC)) != 0) {
        fatal("nng_recv", rv);
    }
    std::printf("CLIENT: GOT RESPONSE\n");

    if (sz == sizeof(uint64_t)) {
        uint64_t now;
        GET64(buf, now);
        printf("CLIENT: RECEIVED DATE: ");
        showdate((time_t)now);
    } else {
        printf("CLIENT: GOT WRONG SIZE!\n");
    }

    nng_free(buf, sz);
    nng_close(sock);
    return 0;
}

void task_creator(const char *dist_url, const char *collect_url) {
    nng_socket dist_sock, collect_sock;
    int err;

    err = nng_push0_open(&dist_sock);
    if (err) {
        fatal("nng_push0_open", err);
    }
    printf("TASK_CREATOR: dist_socket open!\n");
    err = nng_pull0_open(&collect_sock);
    if (err) {
        fatal("nng_pull0_open", err);
    }
    printf("TASK_CREATOR: collect_socket open!\n");

    err = nng_listen(dist_sock, dist_url, NULL, 0);
    if (err) {
        fatal("nng_listen", err);
    }
    printf("TASK_CREATOR: dist socket listening!\n");
    err = nng_listen(collect_sock, collect_url, NULL, 0);
    if (err) {
        fatal("nng_listen", err);
    }
    printf("TASK_CREATOR: collect socket listening!\n");

    std::thread collector([&]() {
        printf("COLLECTOR running\n");
        int err;
        char *buf;
        size_t sz = 512;
        while (true) {
            err = nng_recv(collect_sock, &buf, &sz, NNG_FLAG_ALLOC);
            if (err) {
                fatal("nng_recv", err);
            }
            printf("TASK_COLLECTOR: got response\n");
            nng_free(buf, sz);
        }
    });

    uint64_t c = 0;
    std::vector<char> buf(sizeof(c));
    while (true) {
        PUT64(buf.data(), c);
        c++;
        err = nng_send(dist_sock, buf.data(), buf.size(), 0);
        if (err) {
            fatal("nng_send", err);
        }
        printf("TASK_CREATOR: sent one task!\n");

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    collector.join();
}

#include <sstream>
void worker(const char *dist_url, const char *collect_url) {
    std::stringstream tid;
    tid << std::this_thread::get_id();
    auto id = tid.str();
    nng_socket dist_sock, collect_socket;
    int err;

    err = nng_pull0_open(&dist_sock);
    if (err) {
        fatal("nng_pull0_open", err);
    }
    printf("WORKER[%s]: dist socket open!\n", id.c_str());

    err = nng_push0_open(&collect_socket);
    if (err) {
        fatal("nng_pull0_open", err);
    }
    printf("WORKER[%s]: collect socket open!\n", id.c_str());

    do {
        err = nng_dial(dist_sock, dist_url, nullptr, 0);
        if (err) {
            printf("WORKER[%s]: dial attempt failed! Reason: %s\n", id.c_str(), nng_strerror(err));
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    } while (err);
    printf("WORKER[%s]: dist socket dialed!\n", id.c_str());

    do {
        err = nng_dial(collect_socket, collect_url, nullptr, 0);
        if (err) {
            printf("WORKER[%s]: dial attempt failed! Reason: %s\n", id.c_str(), nng_strerror(err));
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    } while (err);
    printf("WORKER[%s]: collect socket dialed!\n", id.c_str());

    while (true) {
        char *buf;
        size_t sz = 16;
        err = nng_recv(dist_sock, &buf, &sz, NNG_FLAG_ALLOC);
        if (err) {
            fatal("nng_recv", err);
        }
        uint64_t c;
        GET64(buf, c);
        printf("WORKER[%s]: received message! %llu\n", id.c_str(), c);

        err = nng_send(collect_socket, buf, sz, NNG_FLAG_ALLOC);
        if (err) {
            fatal("nng_send", err);
        }
        printf("WORKER[%s]: processed task!\n", id.c_str());
    }

    nng_close(dist_sock);
    nng_close(collect_socket);
}

#include <iostream>
int main(const int argc, const char **argv) {
    if (argc == 3) {
        std::thread distributor(task_creator, argv[1], argv[2]);
        std::vector<std::thread> tds;
        for (uint i = 0; i < std::thread::hardware_concurrency(); ++i) {
            tds.emplace_back(std::thread(worker, argv[1], argv[2]));
        }
        distributor.join();
        for (auto &t : tds) {
            t.join();
        }
    }
    fprintf(stderr, "Usage: %s <URL> <URL2>\n", argv[0]);
    return 1;
}