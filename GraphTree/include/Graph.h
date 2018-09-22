//
// Created by sergysanjj on 19.09.18.
//

#ifndef GRAPHTREE_GRAPH_H
#define GRAPHTREE_GRAPH_H

#include <cstdlib>
#include <iostream>
#include <array>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <stack>
#include <type_traits>
#include <typeinfo>


namespace GraphTree {

    // TODO: separate errors handler/message printer to a different file/class.
    // TODO: mb add Error return value from functions.
    enum Errors {
        BadAlloc = 0,
        NoSuchVertexNumber = 1
    };

    enum Print {
        vertices = 0,
        data = 1
    };

    class Empt {};

    // Inherit from this class and rewrite print() method.
    template <typename T>
    class Printable : public Empt{
    public:
        explicit Printable(T value): data(value) {}

        ~Printable() = default;

        virtual void print() = 0;
        T getData() { return  data; }
    private:
    protected:
        T data;
    };


    template <typename T>
    void printSpecificData(T& arg)
    {
        //static_assert(std::is_base_of<Printable<T>, T>::value, "Data class is not derived from Printable.");
        if (dynamic_cast<Printable<T> >(&arg))
            arg.print();
        else {
            std::cout << "Data class is not derived from Printable.";
            return;
        }
    }





    template<typename T>
    class Vertex {

    public:
        Vertex<T>() : vertexData() {}

        explicit Vertex<T>(T data) : vertexData(data) {}

        Vertex &operator=(const Vertex<T> &rsv) {
            vertexData = static_cast<T>(rsv.vertexData);
            return *this;
        }

        Vertex &operator=(const T &value) {
            vertexData = static_cast<T>(value);
            return *this;
        }

        T getData()  { return  vertexData; }

        ~Vertex<T>() = default;

    private:

        T vertexData;
    };


    template<typename T>
    class Graph {

    public:

        // Create an empty graph with n verticies.
        explicit Graph(std::size_t n );

        explicit Graph():N(0),E(0) {}

        explicit Graph(const std::vector<T> &values);

        ~Graph() = default;

        // TODO: write copy constructor, assigment operator

        Graph &operator=(const Graph<T> &rhs);

        std::size_t size() const { return N; }

        std::size_t edgeCount() const { return E; }

        void print(Print option);

        // Call as .addVertex(Vertex(data));
        void addVertex(const Vertex<T> &v);

        // Connect u wirh v (numeration from 0, DOESN'T supports multiedges).
        void addEdge(std::size_t u, std::size_t v);

        // Returns pointer to the new graph that contains Spanning Tree forest of .self
        Graph<T> *getSpanningTree();

    private:
        void spanningDFS(Graph<T> *resGraph, std::vector<bool> &visited, std::size_t v);

    protected:

        void copyVertexData(Graph<T> *rhs);

        // Vertex number.
        std::size_t N;
        // Edge number.
        std::size_t E;
        // Here will be stored pointers to verts.
        std::vector<Vertex<T> > vertexList;
        // Adjacency list.
        std::vector<std::set<std::size_t> > adjList;  // TODO: think once more about using hash map insted
    };

/********--------Implementation--------********/

    template<typename T>
    Graph<T>::Graph(std::size_t n): N(n), E(0) {
        vertexList.resize(N);
        adjList.resize(N);

        for (auto vert : vertexList) {
            vert = Vertex<T>();
        }
    }

    template<typename T>
    Graph<T>::Graph(const std::vector<T> &values): N(values.size()), E(0) {
        vertexList.reserve(values.size());
        adjList.resize(values.size());

        for (std::size_t i = 0; i < N; i++) {
            vertexList.push_back(Vertex(values[i]));
        }
    }


    template<typename T>
    void Graph<T>::addVertex(const GraphTree::Vertex<T> &v) {
        vertexList.push_back(v);
        adjList.resize(adjList.size() + 1);
        N++;
    }


    template<typename T>
    void Graph<T>::addEdge(std::size_t u, std::size_t v) {
        if (u < N && v < N) {
            // Won't insert once more if and create multiedg.
            if (adjList[u].find(v) == adjList[u].end()) {
                adjList[u].insert(v);
                adjList[v].insert(u);
                E++;
            }
        } else {
            std::cerr << "\nError: u or v vert not in graph.\n";
        }
    }


    template<typename T>
    void Graph<T>::print(Print option) {
        if (option == Print::vertices) {
            std::cout << "Adjecency list:\n";
            for (std::size_t i = 0; i < N; i++) {
                std::cout << " " << i << ": ";
                if (adjList[i].empty())
                    std::cout << "--empty--";
                else {
                    for (auto connection : adjList[i]) {
                        std::cout << connection << " ";
                    }
                }
                std::cout << ";\n";
            }
        } else if(option == Print::data) {
            if (std::is_base_of<Printable<T>, T>::value)
            {
                std::cout << "Adjecency list:\n";
                for (std::size_t i = 0; i < N; i++) {
                    vertexList[i].getData().print();
                    std::cout << ": ";
                    if (adjList[i].empty())
                        std::cout << "--empty--";
                    else {
                        /*  for (auto* connection : vertexList[i]) {
                              printSpecificData(connection->getData());

                              std::cout << " ";
                          }*/
                    }
                    std::cout << ";\n";
                }
            }

            else {
                std::cout << "Data class is not derived from Printable.\n";
                return;
            }


        }

    }

    template<typename T>
    void Graph<T>::copyVertexData(Graph<T> *rhs) {
        vertexList.clear();
        vertexList = rhs->vertexList;
        adjList.clear();
        adjList.resize(rhs->size());
        N = rhs->size();
        E = 0;

    }

    template<typename T>
    Graph<T> *Graph<T>::getSpanningTree() {
        using std::vector, std::array;

        std::vector<bool> visited(N, false);
        auto *resTree = new Graph<T>();

        resTree->copyVertexData(this);

        for (std::size_t i = 0; i < N; i++) {
            if (!visited[i]) {
                spanningDFS(resTree, visited, i);
            }
        }

        return resTree;
    }

    template<typename T>
    void Graph<T>::spanningDFS(Graph<T> *resGraph, std::vector<bool> &visited, std::size_t v) {
        // We will do it non-recursive.

        // Emulate recursion with stack.
        std::stack<std::size_t> S;
        S.push(v);

        while (!S.empty()) {
            v = S.top();
            S.pop();

            if (!visited[v]) {

                visited[v] = true;
            }

            for (auto w: this->adjList[v]) {
                if (!visited[w]) {
                    resGraph->addEdge(v, w);
                    S.push(w);
                    visited[w] = true;
                }
            }


        }

    }

    template<typename T>
    Graph<T> &Graph<T>::operator=(const Graph<T> &rhs) {
        vertexList.clear();
        adjList.clear();

        vertexList = rhs.vertexList;
        adjList = rhs.adjList;

        N = rhs.size();
        E = rhs.size();


        return *this;
    }


}

#endif //GRAPHTREE_GRAPH_H
