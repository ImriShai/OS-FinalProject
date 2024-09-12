/*
** pollserver.c -- a cheezy multiperson chat server
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <algorithm>
#include <string>
#include <iostream>
#include <sstream>
#include "GraphObj/graph.hpp"
#include "MST/MST_Strategy.hpp"
#include "MST/MST_Factory.hpp"
#include "LFP/LFP.hpp"
#include <mutex>
#include <shared_mutex>
#include "ServerUtils/serverUtils.hpp"


#define NUM_THREADS 4 // Number of threads in LFP
#define PORT "9036"   // Port we're listening on
#define WELCOME_MSG_SIZE 480

using namespace std;

// void* designP = nullptr;
LFP lfp(NUM_THREADS); // Create an instance of LFP
// bool designPattern = false;  // Leader-Folower == false, Pipeline == true

// Mutex for the graph
std::shared_mutex graphMutex;

std::pair<std::string, Graph *> MST(Graph *g, int clientFd, const std::string& strat) // many to do here
{
    // implementing Leader-Follower with global variable "lfp":
    lfp.addTask([clientFd, strat, g]() {
        
        // Locking the graph
        std::shared_lock<std::shared_mutex> lock(graphMutex);
        std::cout << "User " << clientFd << " requested to find MST of the Graph, locking graph" << std::endl;
        // Perform the operation
        std::string msg = "Client " + std::to_string(clientFd) + " requested to find MST of the Graph" + "\n";
        msg+= "MST Strategy: " + strat + "\n";
        Graph mst = (*MST_Factory::getInstance()->createMST(strat))(g);
        msg += "MSTs' stats: \n" + mst.stats();
        send(clientFd, msg.c_str(), msg.size(), 0);
        std::cout << "User " << clientFd << " finished finding MST of the Graph, unlocking graph" << endl;
    });
    return {"", nullptr};
}


// Main
int main(void)
{
    lfp.start();          // Start the threads in LFP

    Graph *g = nullptr;
    int listener; // Listening socket descriptor
    pair<string, Graph *> ret;
    string action;
    string actualAction;
    int m, n, weight;
    string strat;
    map<int, bool> connections_in_use;
    char welcomeMsg[WELCOME_MSG_SIZE] = 
        "Welcome to the server!\n"
        "This server can perform the following actions:\n"
        "1. Create a new graph: newgraph n m where \"n\" is the number of vertices and \"m\" is the number of edges.\n"
        "2. Add an edge to the graph: newedge n m w where \"n\" and \"m\" are the vertices and \"w\" is the weight of the edge.\n"
        "3. Remove an edge from the graph: removeedge n m where \"n\" and \"m\" are the vertices.\n"
        "4. Find the Minimum Spanning Tree of the graph: mst strat -  where strat is either 'prim' or 'kruskal'\n";
       

    int newfd;                          // Newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // Client address
    socklen_t addrlen;
    string msg; // Buffer for server result to send
    const vector<string> graphActions = {"newgraph", "newedge", "removeedge", "mst"};
    const vector<string> mstStrats = {"prim", "kruskal"};

    char buf[256]; // Buffer for client data

    char remoteIP[INET6_ADDRSTRLEN];

    // Start off with room for 5 connections (isnt it four? because one is the listener..)
    // (We'll realloc as necessary)
    int fd_count = 0;
    int fd_size = 5;
    struct pollfd *pfds = (struct pollfd *)malloc(sizeof *pfds * (size_t)fd_size);

    // Set up and get a listening socket
    listener = getListenerSocket();

    if (listener == -1)
    {
        fprintf(stderr, "error getting listening socket\n");
        exit(1);
    }

    cout << "pollserver: waiting for connections..." << endl;
    // Add the listener to set
    pfds[0].fd = listener;
    pfds[0].events = POLLIN; // Report ready to read on incoming connection

    fd_count = 1; // For the listener

    // Main loop
    for (;;)
    {
        int poll_count = poll(pfds, (size_t)fd_count, -1);

        if (poll_count == -1)
        {
            perror("poll");
            exit(1);
        }

        // Run through the existing connections looking for data to read
        for (int i = 0; i < fd_count; i++)
        {

            // Check if someone's ready to read
            if (pfds[i].revents & POLLIN)
            { // We got one!!

                if (pfds[i].fd == listener)
                { // If listener is ready to read, handle new connection

                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);

                    if (newfd == -1)
                    {
                        perror("accept");
                    }
                    else
                    {
                        add_to_pfds(&pfds, newfd, &fd_count, &fd_size);
                        connections_in_use[newfd] = false; // Add the new connection to the set of connections

                        printf("pollserver: new connection from %s on "
                               "socket %d\n",
                               inet_ntop(remoteaddr.ss_family,
                                         getInAddr((struct sockaddr *)&remoteaddr),
                                         remoteIP, INET6_ADDRSTRLEN),
                               newfd);
                        if (send(newfd, welcomeMsg, sizeof(welcomeMsg), 0) < 0)
                        {
                            perror("send");
                        }
                    }
                }
                else
                { // handle existing connection

                    if (connections_in_use[pfds[i].fd])
                    { // If the connection is in use, skip it
                        continue;
                    }
                    connections_in_use[pfds[i].fd] = true;             // Mark the connection as in use
                    int nbytes = recv(pfds[i].fd, buf, sizeof buf, 0); // receiving the msg from the client
                    int sender_fd = pfds[i].fd;

                    if (nbytes <= 0)
                    { // Got error or connection closed by client

                        if (nbytes == 0)
                            printf("pollserver: socket %d hung up\n", sender_fd);
                        else
                            perror("recv");

                        close(pfds[i].fd);
                        del_from_pfds(pfds, i, &fd_count);
                        connections_in_use.erase(pfds[i].fd);
                    }
                    else
                    {
                        parseInput(buf, nbytes, n, m, weight, strat, action, actualAction, graphActions, mstStrats);
                    }

                    cout << "Action received: " << actualAction << endl;
                    pair<string, Graph *> result = handleInput(g, action, sender_fd, actualAction, n, m, weight, strat);
                    string msg = result.first;
                    if (result.second != nullptr) {
                        g = result.second;
                    }
                    char *msg_buf = new char[msg.length() + 1];
                    strcpy(msg_buf, msg.c_str());
                    nbytes = msg.length();
                    for (int j = 0; j < fd_count; j++) {
                        int dest_fd = pfds[j].fd;
                        if (dest_fd != listener && (dest_fd != sender_fd || actualAction != "message")) {
                            if(msg.size() == 0) {
                                continue;
                            }
                            if (send(dest_fd, msg_buf, (size_t)nbytes, 0) == -1) {
                                perror("send");
                            }
                        }
                    }
                    delete[] msg_buf;
                    connections_in_use[sender_fd] = false;  // Mark the connection as free so the next event can be processed by main thread
                    cout << "Connection " << sender_fd << " is now free" << endl;
                    msg = "";
                }
            } // END handle data from client
        } // END got ready-to-read from poll()
    } // END looping through file descriptors
    // END for(;;)--and you thought it would never end!

    return 0;
}
