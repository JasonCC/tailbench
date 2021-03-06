/** $lic$
 * Copyright (C) 2016-2017 by Massachusetts Institute of Technology
 *
 * This file is part of TailBench.
 *
 * If you use this software in your research, we request that you reference the
 * TaiBench paper ("TailBench: A Benchmark Suite and Evaluation Methodology for
 * Latency-Critical Applications", Kasture and Sanchez, IISWC-2016) as the
 * source in any publications that use this software, and that you send us a
 * citation of your work.
 *
 * TailBench is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.
 */

#include "client.h"
#include "helpers.h"

#include <assert.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <csignal>
#include <iostream>
#include <string>
#include <vector>
#include <csignal>

void* send(void* c) {
    NetworkedClient* client = reinterpret_cast<NetworkedClient*>(c);

    while (true) {
//	std::cerr << "client begin sending request " << std::endl;
        Request* req = client->startReq();
	
        if (!client->send(req)) {
            std::cerr << "[CLIENT] send() failed : " << client->errmsg() \
                << std::endl;
            std::cerr << "[CLIENT] Not sending further request" << std::endl;
            break; // We are done
        }
//	std::cerr << "client finish sending request " << std::endl;
    }

    return nullptr;
}

void* recv(void* c) {
    NetworkedClient* client = reinterpret_cast<NetworkedClient*>(c);
   // std::cerr << "client begin to recv" << std::endl;
    Response resp;
    while (true) {
        if (!client->recv(&resp)) {
            std::cerr << "[CLIENT] recv() failed : " << client->errmsg() \
                << std::endl;
            return nullptr;
        }
       // std::cerr << "client begin to recv reach here 1" << std::endl;
        if (resp.type == RESPONSE) {
            client->finiReq(&resp);
	 //   std::cerr<<"reach reponse is RESPONSE" << std::endl;
        } else if (resp.type == ROI_BEGIN) {
            client->startRoi();
	 //   std::cerr << "reach reponse is ROI_BEGIN" << std::endl;
        } else if (resp.type == FINISH) {
            client->dumpAllStats();
            syscall(SYS_exit_group, 0);
        } else {
            std::cerr << "Unknown response type: " << resp.type << std::endl;
//	    client->dumpStats();
            return nullptr;
        }
    }
}

int main(int argc, char* argv[]) {
    std::cout << "----------Starting Clients----------" << '\n';
    int nthreads = getOpt<int>("TBENCH_CLIENT_THREADS", 1);
    std::string server = getOpt<std::string>("TBENCH_SERVER", "");
    int serverport = getOpt<int>("TBENCH_SERVER_PORT", 7000);

    //std::cout << "TESTING: " << "finished parsing parameters\n";
	signal(SIGPIPE, SIG_IGN);
    NetworkedClient* client = new NetworkedClient(nthreads, server, serverport);

    std::cout << "----------NetworkedClient initiated----------" << '\n';

    std::vector<pthread_t> senders(nthreads);
    std::vector<pthread_t> receivers(nthreads);

    for (int t = 0; t < nthreads; ++t) {
        int status = pthread_create(&senders[t], nullptr, send, 
                reinterpret_cast<void*>(client));
        assert(status == 0);
    }

    for (int t = 0; t < nthreads; ++t) {
        int status = pthread_create(&receivers[t], nullptr, recv, 
                reinterpret_cast<void*>(client));
        assert(status == 0);
    }

    for (int t = 0; t < nthreads; ++t) {
        int status;
//	std::cout << "[CLIENT] joining sender" << t << '\n';
        status = pthread_join(senders[t], nullptr);
        assert(status == 0);
//	std::cout << "[CLIENT] joined sender" << t << '\n';

//	std::cout << "[CLIENT] joining receiver" << t << '\n';
        status = pthread_join(receivers[t], nullptr);
        assert(status == 0);
//	std::cout << "[CLIENT] joined receiver" << t << '\n';
    }
//	std::cout << "[Client] send and recv threads exited\n";
	if(!client -> getDumped())
	{
		client->dumpAllStats();
	}
	std::cout << "----------Client exiting----------\n";
    return 0;
}
