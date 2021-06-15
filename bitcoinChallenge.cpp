#include <bits/stdc++.h>
using namespace std;

void write_csv(string fileName, vector<string> blockIds){
    
    //Make the damn file
    ofstream myFile(fileName);
    
    //Write the damn data
    //Get the number of data points in each column 
    for(long long i=0; i<blockIds.size() ; i++)
    {
        myFile << blockIds[i];
        //Change line
        myFile << "\n";
    }

    myFile.close();
}

vector<pair<string, vector<string>>> read_csv(string fileName){
    //Returning variable
    vector<pair<string, vector<string>>> data;
    //Create a input filestream
    ifstream myFile(fileName);
    //Error check
    if(!myFile.is_open())
        throw runtime_error("Could not open");
    //Storing variables
    string line, colName;
    if(myFile.good())
    {
        //Get the first line
        getline(myFile, line);
        //Create a stringstream
        stringstream ss(line);
        //Extrect each column name
        while(getline(ss, colName, ','))
        {
            //Add the column name
            data.push_back({colName, vector<string> {}});
        }
    }
    //Start of data
    while(getline(myFile, line))
    {        
        //Create a stringstream
        stringstream ss(line);
        //Which column does the data belong
        long long colInd = 0;
        //Extract each integer
        //https://www.geeksforgeeks.org/stringstream-c-applications/
        string temp;
        while(getline(ss, temp, ','))
        {
            data.at(colInd).second.push_back(temp);
            //Increment colInd
            colInd++;
        }

        if(line.back() == ',')
             data.at(colInd).second.push_back("");
    }

    myFile.close();
    return data;
}

class transaction
{
    public:
        string id;
        long long fee;
        long long weight;
        vector<string> parents;
};

bool isTransactionValid(set<string> &presentIds, transaction* currentTransaction){
    for(auto parent : currentTransaction->parents)
    {        
        //Check if parents present
        if(presentIds.find(parent) == presentIds.end())
            return false;
    }
    //Default
    return true;
}

vector<string> getParents(string temp){
    vector<string> parents;

    stringstream ss(temp);
    string intermediate;
    while(getline(ss, intermediate, ';'))
    {
        parents.push_back(intermediate);
        
    }

    return parents;
}

int main(){ 
    long long weightLimit = 4000000;
    //Data structures to gather data
    vector<pair<string, vector<string>>> data = read_csv("mempool.csv");   
    map<string, transaction*> umap; 
    //Putting data into our custom data structure
    
    for(long long i=0; i<data.at(0).second.size(); i++)
    {
        auto transact = new transaction();

        transact->id = data.at(0).second.at(i);
        transact->fee = stoi(data.at(1).second.at(i));
        transact->weight = stoi(data.at(2).second.at(i));        

        string temp = data.at(3).second.at(i);
        //We need to get individual parents ids
        vector<string> Parents = getParents(temp);
        transact->parents = Parents;
        //Make a pair
        pair<string, transaction*> p = {data.at(0).second.at(i), transact};
        //Put in umap
        umap[p.first] = p.second;
    }

    //Lets make a set and store the data according to fee
    set< pair<int, transaction*>, greater<pair<int, transaction*>>> transactionSet;
    for(auto x : umap)
        transactionSet.insert({x.second->fee, x.second});
    
    //Lets define our block vector. This will store the ids of transactions that make it into the block
    vector<string> blockIds;

    //Lets define a set to store Ids already in the block for checking parents
    set<string> presentIds;

    //To store current weight of block
    long long blockWeight = 0;

    //To store total fee of block
    long long totalFee = 0;

    while(!transactionSet.empty() && (blockWeight <= weightLimit))
    {
        //This boolean keep track whether there was a change in the block
        bool check = false;
        for(auto itr = transactionSet.begin(); itr != transactionSet.end(); itr++)
        {
            transaction* currentTransaction = (*itr).second;
            //Get details about the current transaction
            long long currentTransactionWeight = currentTransaction->weight;
            long long currentTransactionFee = currentTransaction->fee;
            //Check if the current transaction can be included in the block
            if(isTransactionValid(presentIds, currentTransaction) && blockWeight + currentTransactionWeight <= weightLimit)
            {
                //Update the block weight
                blockWeight += currentTransactionWeight;
                //Update the fee of the block
                totalFee += currentTransactionFee;
                //Add Id in the set
                presentIds.insert(currentTransaction->id);
                //Add in output 
                blockIds.push_back(currentTransaction->id);
                //Remove this transaction from the transactionSet as it is already in the block
                transactionSet.erase(itr);
                //Change the check boolean to continue optimising
                check = true;                
            }
        }
        //If no change, exit the loop
        if(!check)
            break;
    }

    //Display block data
    cout<<"Number of transactions in the block = "<<blockIds.size()<<endl;
    cout<<"Total fee of the block = "<<totalFee<<endl;
    cout<<"Total weight of the block = "<<blockWeight<<"/"<<weightLimit<<endl;
    
    write_csv("block.txt", blockIds);

    return 0;
}