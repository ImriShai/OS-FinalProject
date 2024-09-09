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
#include <algorithm> // Add this line
#include <string>
#include <iostream>
#include <sstream>
#include "GraphObj/graph.hpp"
#include "MST/MST_Strategy.hpp"
#include "MST/MST_Factory.hpp"
#include "LFP.hpp"
#include <mutex>


#define NUM_THREADS 4 // Number of threads in LFP
#define PORT "9036" // Port we're listening on

using namespace std;

// Mutex for the graph
mutex graphMutex;


// Function to convert a string to lowercase
string toLowerCase(string s)
{
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

void initGraph(Graph *g, int m, int clientFd)
{
    cout << "m: " << m << endl;
    string msg = "To create an edge u->v with weight w please enter the edge number in the format: u v w \n";
    if(send(clientFd, msg.c_str(), msg.size(), 0)<0){
        perror("send");
    }
    int stdin_save = dup(STDIN_FILENO); // Save the current state of STDIN
    dup2(clientFd, STDIN_FILENO);      // Redirect STDIN to the socket
    for (int i = 0; i < m; i++)
    { // Read the edges
        size_t u, v, weight;
        cin >> u >> v >> weight;
        Edge e = Edge(g->getVertex(u - 1), g->getVertex(v - 1), weight);
        g->addEdge(e); // Add edge from u to v
    }
    dup2(stdin_save, STDIN_FILENO); // Restore the original STDIN
}

std::vector<std::string> splitStringBySpaces(const std::string &input)
{
    std::istringstream stream(input);
    std::vector<std::string> result;
    std::string temp;

    while (stream >> temp)
    {
        result.push_back(temp);
    }

    return result;
}

void parseInput(char* buf, int nbytes, int &n, int &m, int &weight, string &strat,string& action, string& actualAction, const vector<string>& graphActions, const vector<string>& mstStrats)
{
    buf[nbytes] = '\0';
    action = toLowerCase(string(buf));
    vector<string> tokens = splitStringBySpaces(action);
    if (tokens.size() > 0) {
        actualAction = tokens[0];
    } else {
        actualAction = "emptyMessage";
    }
    if (find(graphActions.begin(), graphActions.end(), actualAction) == graphActions.end()) {
        actualAction = "message";
    } else if (actualAction == "mst") {
        if (tokens.size() > 1) {
            if (find(mstStrats.begin(), mstStrats.end(), tokens[1]) == mstStrats.end()) {
                actualAction = "message";
            } else {
                actualAction = "mst";
                action = tokens[0];
                n = -1;
                m = -1;
                weight = -1;
                strat = tokens[1];
            }
        }
    } else if (actualAction == "newgraph") {
        n = stoi(tokens[1]);
        m = stoi(tokens[2]);
        weight = -1;
    } else if (actualAction == "newedge") {
        n = stoi(tokens[1]);
        m = stoi(tokens[2]);
        weight = stoi(tokens[3]);
    } else {
        n = stoi(tokens[1]);
        m = stoi(tokens[2]);
        weight = -1;
    }
}


unordered_set<Vertex> initVertices(int n)
{
    unordered_set<Vertex> vertices;
    for (size_t i = 0; i < n; i++)
    {
        vertices.insert(Vertex(i));
    }
    return vertices;
}

pair<string, Graph *> newGraph(int n, int m, int clientFd, Graph *g)
{   
    unique_lock<std::mutex> lock(graphMutex); // locking the mutex:
    cout << "Creating a new graph with " << n << " vertices and " << m << " edges" << endl;

    if (g != nullptr)
        delete g;
    unordered_set<Vertex> vertices = initVertices(n);           // Initialize the vertices
    
    g = new Graph(vertices);    // Create a new graph of n vertices
    initGraph(g, m, clientFd); // Initialize the graph with m edges

    string msg = "Client " + to_string(clientFd) + " successfully created a new Graph with " + to_string(n) + " vertices and " + to_string(m) + " edges" + "\n";
    cout<< "Graph created successfully\n";
    return {msg, g};
}

pair<string, Graph *> newEdge(size_t n, size_t m, size_t weight,  int clientFd, Graph *g)
{
    //Locking the graph
    unique_lock<std::mutex> lock(graphMutex); 
    cout << "Adding an edge from " << n << " to " << m << endl;
    // cout << &(*g);
    g->addEdge(Edge(g->getVertex(n - 1), g->getVertex(m - 1), weight)); // Add edge from u to v
    string msg = "Client " + to_string(clientFd) + " added an edge from " + to_string(n) + " to " + to_string(m) + " with weight " + to_string(weight) + "\n";
    
    return {msg, g};
}

pair<string, Graph *> removeedge(int n, int m, int clientFd, Graph *g)
{
    //Locking the graph
    unique_lock<std::mutex> lock(graphMutex);
    cout << "Removing an edge from " << n << " to " << m << endl;
    g->removeEdge(Edge{g->getVertex(n - 1), g->getVertex(m - 1)}); // Remove edge from u to v
    string msg = "Client " + to_string(clientFd) + " removed an edge from " + to_string(n) + " to " + to_string(m) + "\n";
    return {msg, g};
}

pair<string, Graph *> MST(Graph *g, int clientFd, string strat) //many to do here
{
    unique_lock<std::mutex> lock(graphMutex);  //   Locking the graph
    string msg = "Client " + to_string(clientFd) + " requested to find MST of the Graph" + "\n";
    int stdout_save = dup(STDOUT_FILENO); // Save the current state of STDOUT
    int pipefd[2];
    pipe(pipefd);                   // Create a pipe
    dup2(pipefd[1], STDOUT_FILENO); // Redirect STDOUT to the pipe
    close(pipefd[1]);               // Close the write-end of the pipe as it's now duplicated

    // Perform the operation
    Graph mst = (*MST_Factory::getInstance()->createMST(strat))(g);
    cout << mst; // Print the MST stats

    // Restore the original STDOUT
    dup2(stdout_save, STDOUT_FILENO);
    close(stdout_save); // Close the saved STDOUT

    // Read from the pipe
    std::string output;
    char buffer[128];
    ssize_t bytes_read;
    while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[bytes_read] = '\0'; // Null-terminate the string
        output += buffer;
    }
    close(pipefd[0]); // Close the read-end of the pipe
    // Use 'output' as needed
    msg += "MSTs' stats: \n" + output;
    return {msg, nullptr};
}

pair<string, Graph *> handleInput(Graph *g, string action, int clientFd, string actualAction, int n, int m, int w, string strat)
{
    string msg;
    vector<string> tokens = splitStringBySpaces(action);
    if (tokens.size() < 1)
    {
        msg = "User " + to_string(clientFd) + " sent an empty message\n";
        return {msg, nullptr};
    }
    

    if (actualAction == "newgraph")
    { // format: newgraph n m
        return newGraph(n, m, clientFd, g);
    }
    else if (actualAction == "newedge")
    { // format: newedge n m (add an edge from n to m)
        if (g != nullptr)
        {
            return newEdge(static_cast<size_t>(n), static_cast<size_t>(m), static_cast<size_t>(w), clientFd, g);
        }
        else
        {
            msg = "Client " + to_string(clientFd) + " tried to perform the operation but there is no graph\n";
            return {msg, nullptr};
        }
    }
    else if (actualAction == "removeedge")
    { // format: removeedge n m (remove an edge from n to m)
        if (g != nullptr)
        {
            return removeedge(n, m, clientFd, g);
        }
        else
        {
            msg = "Client " + to_string(clientFd) + " tried to perform the operation but there is no graph\n";
            return {msg, nullptr};
        }
    }
    else if (actualAction == "mst")
    { // format: MST
        if (g == nullptr)
        {
            msg = "Client " + to_string(clientFd) + " tried to perform the operation but there is no graph\n";
            return {msg, nullptr};
        }
        else
        {
            return MST(g, clientFd, strat);
        }
    }
    else
    {
        msg = "Client " + to_string(clientFd) + " sent a message:" + action;
        return {msg, nullptr};
    }
}

// Get sockaddr, IPv4 or IPv6:
void *getInAddr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

// Return a listening socket
int getListenerSocket(void)
{
    int listener; // Listening socket descriptor
    int yes = 1;  // For setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo hints, *ai, *p;

    // Get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0)
    {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for (p = ai; p != NULL; p = p->ai_next)
    {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0)
        {
            continue;
        }

        // Lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0)
        {
            close(listener);
            continue;
        }

        break;
    }

    freeaddrinfo(ai); // All done with this

    // If we got here, it means we didn't get bound
    if (p == NULL)
    {
        return -1;
    }

    // Listen
    if (listen(listener, 10) == -1)
    {
        return -1;
    }

    return listener;
}

// Add a new file descriptor to the set
void add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size)
{
    // If we don't have room, add more space in the pfds array
    if (*fd_count == *fd_size)
    {
        *fd_size *= 2; // Double it

        *pfds = (struct pollfd *)realloc(*pfds, sizeof(**pfds) * (size_t)(*fd_size));
    }

    (*pfds)[*fd_count].fd = newfd;
    (*pfds)[*fd_count].events = POLLIN; // Check ready-to-read

    (*fd_count)++;
}

// Remove an index from the set
void del_from_pfds(struct pollfd pfds[], int i, int *fd_count)
{
    // Copy the one from the end over this one
    pfds[i] = pfds[*fd_count - 1];

    (*fd_count)--;
}

// Main
int main(void)
{
    LFP lfp(NUM_THREADS);  // Create an instance of LFP
    lfp.start();  // Start the threads in LFP

    Graph *g = nullptr;
    int listener; // Listening socket descriptor
    pair<string, Graph *> ret;
    string action;
    string actualAction;
    int m, n, weight;
    string strat;
    map<int, bool> connections_in_use;

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
        for (int i = 0; i < fd_count; i++){

            // Check if someone's ready to read
            if (pfds[i].revents & POLLIN){ // We got one!!

                if (pfds[i].fd == listener){  // If listener is ready to read, handle new connection

                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);

                    if (newfd == -1){
                        perror("accept");
                    } else { 
                        add_to_pfds(&pfds, newfd, &fd_count, &fd_size);
                        connections_in_use[newfd] = false;   // Add the new connection to the set of connections

                        printf("pollserver: new connection from %s on "
                               "socket %d\n",
                               inet_ntop(remoteaddr.ss_family,
                                         getInAddr((struct sockaddr *)&remoteaddr),
                                         remoteIP, INET6_ADDRSTRLEN),
                               newfd);
                        if (send(newfd, "Welcome to the server!\n", 24, 0) < 0){
                            perror("send");
                        }
                    }
                } else {  // handle existing connection

                    if (connections_in_use[pfds[i].fd]){  // If the connection is in use, skip it
                        continue;
                    }
                    connections_in_use[pfds[i].fd] = true;  // Mark the connection as in use
                    int nbytes = recv(pfds[i].fd, buf, sizeof buf, 0);  // receiving the msg from the client
                    int sender_fd = pfds[i].fd;

                    if (nbytes <= 0) {  // Got error or connection closed by client

                        if (nbytes == 0) printf("pollserver: socket %d hung up\n", sender_fd);
                        else perror("recv");

                        close(pfds[i].fd);
                        del_from_pfds(pfds, i, &fd_count);
                        connections_in_use.erase(pfds[i].fd);

                    } else {
                        parseInput(buf, nbytes, n, m, weight, strat, action, actualAction, graphActions, mstStrats);
                    }
                    
                    // Add task to LFP. NOTE which arguments we are passing by reference
                    lfp.addTask([&g, action, sender_fd, actualAction, n, m, weight, strat, &pfds, &fd_count, &listener, &connections_in_use]() {
                        cout << "Action received: " << actualAction << endl;
                        pair<string, Graph *> result = handleInput(g, action, sender_fd, actualAction, n, m, weight, strat);
                        string msg = result.first;
                        if (result.second != nullptr) {
                            g = result.second;
                        }
                        char *msg_buf = new char[msg.length() + 1];
                        strcpy(msg_buf, msg.c_str());
                        int nbytes = msg.length();
                        for (int j = 0; j < fd_count; j++) {
                            int dest_fd = pfds[j].fd;
                            if (dest_fd != listener && (dest_fd != sender_fd || actualAction != "message")) {
                                if (send(dest_fd, msg_buf, (size_t)nbytes, 0) == -1) {
                                    perror("send");
                                }
                            }
                        }
                        delete[] msg_buf;
                        connections_in_use[sender_fd] = false;  // Mark the connection as free so the next event can be processed by main thread
                        cout << "Connection " << sender_fd << " is now free" << endl;
                    });
                    msg = "";
                   
                }
            } // END handle data from client
        } // END got ready-to-read from poll()
    } // END looping through file descriptors
    // END for(;;)--and you thought it would never end!

    return 0;
}

