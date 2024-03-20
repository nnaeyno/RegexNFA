#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <unordered_set>
#include <set>
#include <stack>

using namespace std;
enum types {ACCEPT, REJECT};
struct node{
    int index;
    map<char, unordered_set<node*> > transitions;
    unordered_set<node*> incoming;
    bool start;
    types type;
    bool visited = false;
    unordered_set<node*> removed;
};
struct nfa{
    vector<node*> nodes;
    unordered_set<node*> accepts;
    node* start;
};

int nodeCount;
int num;

nfa makeTran(char ch){
    node* st = new node;
    st->index = nodeCount;
    st->start = true;
    nodeCount++;
    node* end = new node;
    end->index = nodeCount;
    end->start = false;
    end->incoming.insert(st);
    unordered_set<node*> vec;
    vec.insert(end);
    st->transitions[ch] = vec;
    nodeCount++;
    nfa myNfa;
    myNfa.start=st;
    myNfa.nodes.push_back(st);
    myNfa.nodes.push_back(end);
    myNfa.accepts.insert(end);
    return myNfa;
}

nfa makeUnion(nfa &two, nfa &one){
    node* st = new node;
    st->index = nodeCount;
    nodeCount++;
    unordered_set<node*> vec;
    vec.insert(one.start);
    vec.insert(two.start);
    one.start->incoming.insert(st);
    two.start->incoming.insert(st);
    st->transitions['-'] = vec;
    node* end = new node;
    end->index= nodeCount;
    nodeCount++;
    end->incoming.insert(*(one.accepts.begin()));
    end->incoming.insert(*(two.accepts.begin()));
    unordered_set<node*> vec2;
    vec2.insert(end);
    unordered_set<node*> vec3;
    vec3.insert(end);
    (*(one.accepts.begin()))->transitions['-'] = vec2;
    (*(two.accepts.begin()))->transitions['-'] = vec3;
    nfa myNfa;
    myNfa.accepts.insert(end);
    myNfa.nodes = one.nodes;
    myNfa.nodes.push_back(st);
    myNfa.nodes.insert(myNfa.nodes.end(), two.nodes.begin(), two.nodes.end());
    myNfa.start = st;
    myNfa.nodes.push_back(end);
    return myNfa;
    // map<char, vector<node*> > twoTran = two.start->transitions;
    // for (auto const& x : twoTran){
    //     if(one.start->transitions.find(x.first) == one.start->transitions.end()){
    //         vector<node*> vec;
    //         one.start->transitions[x.first] = vec;
    //     }
    //     for(int m = 0; m < x.second.size(); m++){
    //         one.start->transitions[x.first].push_back(x.second[m]);
    //         x.second[m]->incoming.insert(one.start);
    //     }
    // }
    // remove(two.start);
    // one.nodes.insert(one.nodes.end(), two.nodes.begin(), two.nodes.end());
    // one.accepts.insert(one.accepts.end(), two.accepts.begin(), two.accepts.end());
    // return one;
}

nfa makeConcat(nfa &two, nfa &one){
    (*(one.accepts.begin()))->transitions = two.start->transitions;
    one.accepts = two.accepts;
    one.nodes.insert(one.nodes.end(), two.nodes.begin(), two.nodes.end());
    return one;
}

bool contains(vector<node*> vec, int index){
    for(int i = 0; i < vec.size(); i++){
        if(vec[i]->index == index) 
            return true;
    }
    return false;
}

nfa makeStar(nfa &one){
    node* st = new node;
    st->index = nodeCount;
    nodeCount++;
    unordered_set<node*> vec;
    vec.insert(one.start);
    st->transitions['-'] = vec;
    node* end = new node;
    end->index= nodeCount;
    nodeCount++;
    end->incoming.insert(*(one.accepts.begin()));
    end->incoming.insert(st);
    st->transitions['-'].insert(end);
    unordered_set<node*> vec2;
    vec2.insert(end);
    vec2.insert(one.start);
    (*(one.accepts.begin()))->transitions['-'] = vec2;
    one.start=st;
    one.accepts.clear();
    one.accepts.insert(end);
    one.nodes.push_back(st);
    one.nodes.push_back(end);
    return one;
}

nfa makeEmpty(){
    node* st = new node;
    st->index = nodeCount;
    st->start = true;
    st->type = REJECT;
    nodeCount++;
    node* end = new node;
    end->index = nodeCount;
    end->start = false;
    end->type = ACCEPT;
    end->incoming.insert(st);
    unordered_set<node*> vec;
    vec.insert(end);
    st->transitions['-'] = vec;
    nodeCount++;
    nfa myNfa;
    myNfa.start=st;
    myNfa.nodes.push_back(st);
    myNfa.nodes.push_back(end);
    myNfa.accepts.insert(end);
    return myNfa;
}

bool isChar(char ch){
    return (ch <= 'z' && ch >= 'a') || (ch >= '0' && ch <= '9') || ch == '-';
}
int priority(char ch){
    if(ch == '*'){
        return 3;
    }
    if(ch == '.'){
        return 2;
    }
    if(ch == '|'){
        return 1;
    }
    return 0;
}
bool needConcat(char one, char two){
     if(isChar(one) && isChar(two)){
            return true;
        } else if (isChar(one) && two == ')'){
           return true;
        } else if(isChar(one) && two== '*')
           return true;
        else if(isChar(two) && one == '(')
            return true;
        else if(one == '(' && two == ')')
            return true;
        else if (two == '*' && one == '(')
            return true;
        else 
            return false;

}
string addConcat(string& str){
    string result= "";
    result += str[0];
    for(int i = 0; i < str.length(); i++){
        if(i == 0){
            if(str[i] == '(' && str[i+1] == ')'){
                result = '-';
                str[i+1] = '-';
                i++;
            }
        } else {
            if(i < str.length()-1 && str[i+1] == ')' && str[i] == '('){
                if(needConcat(str[i], str[i-1]))
                    result += '.';
                result += '-';
                i++;
            } else {
                if(needConcat(str[i], str[i-1])){
                    result += '.';
                }
                result += str[i];
                }
        }
    }
    return result;
}

string makePostFix(string& str){
    string result = "";
    stack<char> st;
    for(int i = 0; i < str.length(); i++){
       
        if(isChar(str[i])){
          
            result += str[i];
          
        }
        else if(str[i] == '('){
            st.push(str[i]);
        }else if(str[i] == ')'){
            while(!st.empty() && st.top() != '('){
                result += st.top();
                st.pop();

            }
            st.pop();
        } else {
            while(!st.empty() && priority(str[i]) <= priority(st.top())){
                result += st.top();
                st.pop();        
            }
            st.push(str[i]);
        }
    }
    while(!st.empty()){
        result += st.top();
        st.pop();
    }
    return result;
}

void addTrans(node* curr, node* hereFromEpsilon, nfa& finalAnswer){
    bool accept = false;
    for(auto const& x : hereFromEpsilon->transitions){
        if(curr->transitions.count(x.first) == 0){
            unordered_set<node*> st;
            curr->transitions[x.first] = st;
        }
        for(auto const& f : x.second){
            if(x.first == '-' && f == curr)
                continue;
            curr->transitions[x.first].insert(f);
            
        }  
    }
    
}


unordered_set<node*> nodes2;
void removeEpsilon(node* curr, nfa& finalAnswer){
    if(curr->visited)
        return;
    map<char, unordered_set<node*> > mp (curr->transitions.begin(), curr->transitions.end());
    for(auto const& firstNode : mp){
        if(firstNode.first == '-' && !curr->transitions['-'].empty()){
            unordered_set<node*> moves(curr->transitions['-'].begin(), curr->transitions['-'].end());
        //    moves.insert(curr->transitions.begin(), curr->transitions.end());

            for(auto const& hereFromEpsilon : moves){
                curr->transitions['-'].erase(hereFromEpsilon);
                addTrans(curr, hereFromEpsilon, finalAnswer);
                if(finalAnswer.accepts.count(hereFromEpsilon) != 0)
                    finalAnswer.accepts.insert(curr);
                if(curr->removed.count(hereFromEpsilon) != 0)
                    continue;
                curr->removed.insert(hereFromEpsilon);
                removeEpsilon(curr, finalAnswer);
            }
        }else{
            curr->visited = true;
            unordered_set <node*> nd(curr->transitions[firstNode.first].begin(), curr->transitions[firstNode.first].end());
            for(auto const& x : nd){
                removeEpsilon(x, finalAnswer);
            }
        }
    }
    curr->visited = true;
 //   cout << curr << endl;
    nodes2.insert(curr);
}

void work(){
   // nodes2.clear();
    nodeCount = 0;
    num = 0;
    nodeCount = 0;
    string curr;
    cin >> curr;
    string newCurr = addConcat(curr);
   // cout << newCurr << endl;
    newCurr = makePostFix(newCurr);
  //  cout << newCurr << endl;
    stack<nfa> st;
    for(int i = 0; i < newCurr.length(); i++){
        char ch = newCurr[i];
        if(isChar(ch)){
            st.push(makeTran(ch));
        }
        if(ch == '.'){
            nfa first = st.top();
            st.pop();
            nfa myNfa = makeConcat(first, st.top());
            st.pop();
            st.push(myNfa);
        }
        if(ch == '|'){
            nfa first = st.top();
            st.pop();
            nfa myNfa = makeUnion(first, st.top());
            st.pop();
            st.push(myNfa);
           
        }
        if(ch == '*'){
            nfa myNfa = makeStar(st.top());
            st.pop();
            st.push(myNfa);
        }
    }
    nfa finalAnswer = st.top();
  //  finalAnswer.accepts.insert(finalAnswer.accepts.end(), finalAnswer.accepts[0]->incoming.begin(), finalAnswer.accepts[0]->incoming.end());
  //  removeEpsilon(finalAnswer);
    removeEpsilon(finalAnswer.start, finalAnswer);
    int numAccepts = 0;
    int numStates = 0;
    int numTransitions = 0;
    string acceptingInd = "";
    int numsForStates = 1;
    string finalOutput = "";

    for(auto const& i : nodes2){
       
        if(i == finalAnswer.start)
            i->index = 0;
        else
            i->index = numsForStates++;
    }
    string startState = "";
    int startTran = 0;
    for(auto const& i : nodes2){
        numStates++;
        int Transitions = 0;
        string curr = "";
        bool start = false;
        if(i->index == 0)
            start = true;
        for (auto const& x : i->transitions){
            if(x.first == '-')
                continue;
            for(auto const& m : x.second){
                numTransitions++;
                if(start){
                    string empty = "";
                    empty+=x.first;
                    startState +=  " " + empty + " " + to_string(m->index);
                    startTran++;
                }
                else{
                    string empty = "";
                    empty+=x.first;
                    curr += " " + empty + " " + to_string(m->index);
                    Transitions++;
                }
            }
        }
        if(start)
            continue;
        if(finalOutput == "")
            finalOutput += to_string(Transitions) + curr;
        else 
            finalOutput += '\n' + to_string(Transitions) + curr;
    }
    for(auto const& i : finalAnswer.accepts){
        if(nodes2.count(i) == 0)
           continue;
        numAccepts++;
        if(acceptingInd == "") acceptingInd += to_string(i->index);
        else
            acceptingInd += " " + to_string(i->index);
    }
    cout << numStates << " " << numAccepts << " " << numTransitions << endl;
    cout << acceptingInd << endl;
    cout << startTran << startState << endl;
    cout << finalOutput << endl;
    // cout << "accepts" << endl;
    // for(auto const& i : finalAnswer.accepts){
    //     if(nodes2.find(i) != nodes2.end())
    //        continue;
    //     cout << i->index << " ";
    // }
    // cout << endl;
    // for(auto const& i : finalAnswer.nodes){
    //     if(deleted.find(i) != deleted.end())
    //         continue;
    //     cout << "from " << i->index << endl;
    //     for (auto const& x : i->transitions){
    //         // if(x.first == '-')
    //         //     continue;
    //         cout << x.first;
    //         for(auto const& m: x.second){
    //             cout<< ":" << m->index;
    //         }
    //         cout << endl;
    //     }
    // }
    // cout << finalAnswer.start->index << " start" << endl;
}
int main() {
    work();
}


// int main(){
//     string input;
//     cin >> input;

//     input = modifyInput(input);
//     //cout << input << endl;
//     nfa result = buildNfa(input);

//     //index nodes
//     int index = 1;
//     for(int i = 0; i < nodes.size(); i++){
//         if(nodes[i] == result.start){
//             result.start->index = 0;
//         } else {
//             nodes[i]->index = index;
//             index++;
//         }

//     }

//     // output 
//     cout << nodeNum << " " << acceptNum << " " << moveNum << endl;
//     for(int i = 0; i < nodes.size(); i++){
//         if(nodes[i]->accept == true){
//             cout << nodes[i]->index << " ";
//         }
//     }
//     cout << endl;
//     printNodeMoves(result.start);
//     for(int i = 0; i < nodes.size(); i++){
//         if(nodes[i] != result.start) printNodeMoves(nodes[i]);
//     }

//     return 0;
// }