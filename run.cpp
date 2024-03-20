#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <unordered_set>
#include <set>
#include <stack>
#include <string>
#include <sstream>
#include <queue>

using namespace std;
struct node{
    int index;
    map<char, unordered_set<node*> > transactions;
};

map<int, node* > nodes;


unordered_set<int> accepts;

string simulateNfa(string& input) {
    string output = "";
    queue<node*> que;
    int ind = 1;
    int num = 0;
    for(auto const& x : nodes[0]->transactions[input[0]]){
        que.push(x);
        num++;
    }
    while(ind <= input.size()){
        char ch = 'N';
        int numsec = 0;
        for(int i = 0; i < num; i++){
            if(accepts.find(que.front()->index) != accepts.end()){
                ch = 'Y';
            }
            if(ind == input.size()){
                que.pop();
                continue;
            }
            for(auto const& x : nodes[que.front()->index]->transactions[input[ind]]){
                que.push(x);
                numsec++;
            }
            que.pop();
       }
       output += ch;
       ind++;
       num = numsec;
    }
    return output;
}
string work(){
    nodes.clear();
    accepts.clear();
    string input;
    cin >> input;
    int numStates;
    int numAccepts;
    int numTran;
    cin >> numStates;
    cin >> numAccepts;
    cin >> numTran;
    int accept;
    for(int i = 0; i < numAccepts; i++){
        cin >> accept;
        if(nodes.find(accept) == nodes.end()){
                node* tr = new node;
                tr->index = accept;
                nodes[accept] = tr;
        }
        accepts.insert(nodes[accept]->index);
    }
    int trans;
    for(int i = 0; i < numStates; i++){
        cin >> trans;
        char ch;
        int ind;
        node* nd = new node;
        nd->index = i;
        nodes[i] = nd;
        for(int j = 0; j < trans; j++){
            cin >> ch;
            cin >> ind;
            if(nd->transactions.find(ch) == nd->transactions.end()){
                unordered_set<node*> st;
                nd->transactions[ch] = st;
            }
            if(nodes.find(ind) == nodes.end()){
                node* tr = new node;
                tr->index = ind;
                nodes[ind] = tr;
            }
            nd->transactions[ch].insert(nodes[ind]);
        }
    }
    // for(auto const& x : accepts){
    //     cout << x->index << endl;
    // }
    //cout << "done" << endl;
    // for(auto const& acc : nodes){
    //     cout << acc.first << " to "; 
    //     for(auto const& x : nodes[acc.first]->transactions){
    //         cout << x.first << " with  ";
    //         for(auto const&y : x.second)
    //             cout << y->index << " ";
    //         cout << endl;
    //     }
    // }
    return simulateNfa(input);
}
int main() {
   cout << work() << endl;
}