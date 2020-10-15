#include <bits/stdc++.h>
using namespace std;
const int inf = 1e9+7, SIZE = 105;
int solve(int source, int destination,int channelID,int isSwithingAllowd);
void modifiedDSDV(int channelID, int nodeNumber, int isSwithingAllowed);
void generateGraph(int totalNode,int channel);
void simulateMySystem(void);
void clearAll(void);
void clearRoutingTable(int nodeNumber,int isSwithingAllowed);
void printGraph(int nodeNumber);
struct node{
   vector <int> neighbourList, channelList[SIZE], distance;
   int hasConnectionWith[SIZE][12];
}Network[SIZE];

struct node2
{
    int from, to, channel, cost;
    node2(int from, int to, int channel, int cost)
    {
        this->from = from;
        this->to = to;
        this->channel = channel;
        this->cost = cost;
    }
    bool operator <(const node2 &p)const {return p.cost<cost;};
};
struct road{
    int from,to,channel;
    road(int from,int to,int channel)
    {
        this->from = from;
        this->to = to;
        this->channel = channel;
    }
};

vector <road> Graph;
unordered_set <int> channelsOfSource;

int totalNode,totalCost,finalCost;
int routingTable[101][101][11];
int isConnected[SIZE];
int hasRoad[SIZE][SIZE];
void generateGraph(int totalNode,int channel)
{
    int n,i,j,k,neighbourNo,c,d,temp,source,destination;
        memset(hasRoad,0,sizeof(hasRoad));
        for(i=1;i<=totalNode;i++)
        {
            if(totalNode>2)
                neighbourNo = 1+rand()%min(3,totalNode-1);
            else
                neighbourNo = 1;
            set<int>neighbourSet;
            for(j=1;j<=totalNode;j++)
            {
                if(hasRoad[i][j]==1)
                {
                    neighbourNo--;
                    neighbourNo = max(0,neighbourNo);

                }
            }
            while(neighbourSet.size()<neighbourNo)
            {
                n= 1+rand()%totalNode;
                if(hasRoad[i][n]==0&&n!=i)
                {
                    neighbourSet.insert(n);
                    hasRoad[i][n]=hasRoad[n][i]=1;
                }

            }
            //printf("Neighbour Set generation end\n");
            set<int>::iterator itt;
            for(itt=neighbourSet.begin();itt!=neighbourSet.end();++itt)
            {

                d=1+rand()%1007;
                Network[i].neighbourList.push_back(*itt);
                Network[*itt].neighbourList.push_back(i);
                Network[i].distance.push_back(d);
                Network[*itt].distance.push_back(d);
                set<int>channelSet;
                int channelNumber = 1+rand()%channel;
                while(channelSet.size()!=channelNumber)
                {
                    channelSet.insert(1+rand()%10);
                }
                set<int>::iterator itt2;
                for(itt2=channelSet.begin();itt2!=channelSet.end();++itt2)
                {
                    Network[i].channelList[*itt].push_back(*itt2);
                    Network[*itt].channelList[i].push_back(*itt2);
                    Network[i].hasConnectionWith[*itt][*itt2]=1;
                    Network[*itt].hasConnectionWith[i][*itt2]=1;
                }
            }
        }

}


void simulateMySystem(void)
{
    int i, j, switching, withSwithcing, withoutSwitching, n, dsdvWithoutSwitching, dsdvWithSwitching;
    int nodeNumber = 10;
    for(int channelNumber=2;channelNumber<=10;channelNumber++)
    {
            withSwithcing=0;
            withoutSwitching=0;
            dsdvWithoutSwitching=0;
            dsdvWithSwitching=0;
            for(int numberOfSimulation =1; numberOfSimulation<=100;numberOfSimulation++)
            {

                clearAll();

                generateGraph(nodeNumber,channelNumber);
                n = Network[1].neighbourList[0];
                 withSwithcing+=solve(1,nodeNumber,Network[1].channelList[n][0],1);

                 channelsOfSource.clear();
                 for(i=0;i<Network[1].neighbourList.size();i++)
                 {
                    n = Network[1].neighbourList[i];
                for(j=0;j<Network[1].channelList[n].size();j++)
                 {
                      channelsOfSource.insert(Network[1].channelList[n][j]);
                    }
                 }
               unordered_set<int>::iterator itt;

                for(itt=channelsOfSource.begin();itt!=channelsOfSource.end();++itt)
               {
                   int isPossible = solve(1,nodeNumber,*itt,0);
                   if(isPossible==1)
                     {
                         withoutSwitching++;
                       break;
                   }
                }
                //----------------- code form MST end --------------------------------

                //----------------- code for modified DSDV begin ---------------------
                clearRoutingTable(nodeNumber,1);
                modifiedDSDV(1,nodeNumber,1); // with switching

                if(routingTable[1][nodeNumber][1]!=inf)
                    dsdvWithSwitching++;
                for(int i=1;i<=10;i++)
                {
                    clearRoutingTable(nodeNumber,0);
                    modifiedDSDV(i,nodeNumber,0);
                    if(routingTable[1][nodeNumber][i]!=inf)
                    {

                        dsdvWithoutSwitching++;
                        break;
                    }
                }
                //----------------- code for modified DSDV end -----------------------

            }
            printf("For %d nodes and %d channel: %d %d  | %d %d\n",nodeNumber,channelNumber,withoutSwitching,withSwithcing,dsdvWithoutSwitching,dsdvWithSwitching);
    }
}


void modifiedDSDV(int cID, int nodeNumber, int isSwithingAllowed)
{
    int to,cost,chnl;
    for(int itteration=1;itteration<=nodeNumber;itteration++)
    {
        for(int i=1;i<=nodeNumber;i++)
        {
            for(int j=0;j<Network[i].neighbourList.size();j++)
            {
                to = Network[i].neighbourList[j];
                cost =Network[i].distance[j];
                for(int k=1;k<=nodeNumber;k++)
                {

                    if(isSwithingAllowed==1)
                    {
                        routingTable[i][k][1]=min(routingTable[i][k][1], routingTable[i][to][1]+routingTable[to][k][1]);
                    }
                    else if(Network[i].hasConnectionWith[to][cID]==1)
                    {
                        routingTable[i][k][cID]=min(routingTable[i][k][cID], routingTable[i][to][cID]+routingTable[to][k][cID]);
                    }
                }
            }
        }
    }
}

void printGraph(int nodeNumber)
{
    for(int i=1;i<=nodeNumber;i++)
    {
        for(int j=0;j<Network[i].neighbourList.size();j++)
        {
            printf("%d -> %d %d c =",i,Network[i].neighbourList[j],Network[i].distance[j]);
            for(int k=0;k<Network[i].channelList[Network[i].neighbourList[j]].size();k++)
                printf(" %d",Network[i].channelList[Network[i].neighbourList[j] ][k]);
            printf("\n");
        }
    }
}

void clearAll(void)
{
    for(int i=1;i<=101;i++)
    {
        Network[i].neighbourList.clear();
        Network[i].distance.clear();
        memset(Network[i].hasConnectionWith,0,sizeof(Network[i].hasConnectionWith));
        for(int j=0;j<104;j++)
        {
            Network[i].channelList[j].clear();
        }

    }
}

void clearRoutingTable(int nodeNumber,int isSwithingAllowed)
{
    for(int i=1;i<=nodeNumber;i++)
    {
        for(int j=i;j<=nodeNumber;j++)
        {
            for(int c=0;c<=10;c++)
            {
                if(i==j)
                {
                    routingTable[i][i][c]=0;
                }
                else
                {
                    routingTable[i][j][c]=inf;
                    routingTable[j][i][c]=inf;
                }
            }
        }
    }
    if(isSwithingAllowed==1)
    {
        for(int i=1;i<=nodeNumber;i++)
        {
            for(int j=0;j<Network[i].neighbourList.size();j++)
            {
                int n=Network[i].neighbourList[j];
                for(int c=0;c<=10;c++)
                {
                    routingTable[i][n][c]=Network[i].distance[j];
                }
            }
        }
    }
    else
    {
        for(int i=1;i<=nodeNumber;i++)
        {
            for(int j=0;j<Network[i].neighbourList.size();j++)
            {
                int n=Network[i].neighbourList[j];
                for(int c=0;c<Network[i].channelList[n].size();c++)
                {
                    int ch =Network[i].channelList[n][c];
                    routingTable[i][n][ch]=Network[i].distance[j];
                    routingTable[n][i][ch]=Network[i].distance[j];
                }
            }
        }
    }
}


int solve(int source, int destination,int channelID,int isSwithingAllowed)
{
    /*
    Depth First Search With Stack
    */
    int isPossible = 0;
    memset(isConnected, 0, sizeof isConnected);
    isConnected[source] = 1;
    Graph.clear();
    stack <node2> myStack;
    for(int i=0;i<Network[source].neighbourList.size();i++)
    {
        int to = Network[source].neighbourList[i];
        int cost =Network[source].distance[i];
        int chnl = Network[source].channelList[to][0];
        if(isSwithingAllowed==1)
        {
           myStack.push(node2(source,to,chnl,cost));
        }
        else if(Network[source].hasConnectionWith[to][channelID]==1)
        {
            myStack.push(node2(source,to,channelID,cost));
        }
    }
    while(myStack.empty() != true)
    {

        node2 temp = myStack.top();
        myStack.pop();
        if(isConnected[temp.to]==0)
        {

            Graph.push_back(road(temp.from,temp.to,temp.channel));
            if(temp.to==destination)
                isPossible =1;
            isConnected[temp.to]=1;
            totalCost+=temp.cost;
            for(int i=0;i<Network[temp.to].neighbourList.size();i++)
            {
                int to = Network[temp.to].neighbourList[i];
                int cost =Network[temp.to].distance[i];
                int chnl = Network[temp.to].channelList[to][0];
                if(isSwithingAllowed==1)
                {
                    myStack.push(node2(temp.to,to,chnl,cost));
                }
                else if(Network[temp.to].hasConnectionWith[to][channelID]==1)
                {
                    myStack.push(node2(temp.to,to,channelID,cost));
                }
            }
        }
    }
    if(isPossible==0)
        totalCost = 9999999;
    return isPossible;
}
int main()
{
    srand(time(NULL));
    simulateMySystem();

    return 0;
}

