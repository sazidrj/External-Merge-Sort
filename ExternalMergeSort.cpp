// This program takes 3 inputs - number of records, disk_block_size = no. of records per file, memory block size = no. of files per memory block
//
//
#include<bits/stdc++.h>
using namespace std;

const int MAX = 26;
char alphabet[MAX] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};

// Function to open file
FILE* openFile(char* fileName, char* mode){
    FILE* fp = fopen(fileName, mode);

    if(fp == NULL)
    {
        perror("Error while opening the file. \n");
        exit(EXIT_FAILURE);
    }
    return fp;
}

// Transaction structure to store the object
struct Transaction{
    int id;
    int sale_amount;
    string name;
    int category;

    // Append all the data to string to store in file
    string TrToString(){
        string str = "";

        str += to_string(id) + " ";
        str += to_string(sale_amount) + " ";
        str += name + " ";
        str += to_string(category);

        return str;
    }
};

// convert string back to Transaction struct
Transaction strToTran(string str){
    string token;
    Transaction t;
    stringstream ss(str);
    for(int i = 0; i<4; i++){
        getline(ss, token, ' ');
        if(i == 0)
            t.id = stoi(token);
        else if(i == 1)
            t.sale_amount = stoi(token);
        else if(i == 2)
            t.name = token;
        else
            t.category = stoi(token);
    }
}

// Merge function of simple merge sort
void merge(vector<Transaction> &v, int l, int mid, int mid1, int r){
    int i = l, j = mid1, m = mid, n = r;
    vector<Transaction> b(r-l+1);
    int k = 0;

    while(i<=m && j<=n){
        int x = v[i].sale_amount;
        int y = v[j].sale_amount;

        if(x<=y){
            b[k++] = v[i++];
        }else{
            b[k++] = v[j++];
        }
    }

    while(i<=m){
        b[k++] = v[i++];
    }

    while(j<=n){
        b[k++]=v[j++];
    }

    for(int x = l; x<=r; x++){
        v[x] = b[x-l];
    }
}

// Simple Merge Sort
void mergesort(vector<Transaction> &v, int l, int r){
    if(l<r)
    {
        int mid = l + (r-l)/2;
        mergesort(v,l,mid);
        mergesort(v,mid+1,r);
        merge(v,l,mid,mid+1,r);
    }
}

int log_a_to_b(int a, int b){
    return ceil(log2(a) / log2(b));
}

// This function will divide the large input file into multiple size according to disc block size
void divideFilesInDiskBlockSize(char* input_file, int run_size, int num_ways){
    ifstream infile;
    string str;
    infile.open("input_file.txt");

    FILE* out[num_ways];
    char fileName[1000];

//    for(int i = 0; i<num_ways; i++){
//        snprintf(fileName, sizeof(fileName), "%d.txt", i);
//        out[i] = openFile(fileName,  "w");
//    }

    vector<Transaction> v;

    int count = 0, i = 0, j = 0;
    snprintf(fileName, sizeof(fileName), "%d.txt", j);
    out[j] = openFile(fileName,  "w");

    while(getline(infile, str)){
        count++;

        if(count > run_size){
            fclose(out[j]);
            j++;
            snprintf(fileName, sizeof(fileName), "%d.txt", j);
            out[j] = openFile(fileName,  "w");

            count = 1;
            i++;
        }


        fprintf(out[j], "%s \n", str.c_str());
    }


//    for(int i = 0; i<num_ways; i++){
//        fclose(out[i]);
//    }

    infile.close();

}

// This is initial phase - Sorting individual files
void sortIndividualFile(int i, int append){
    vector<Transaction> v;

    ifstream infile;
    string str;
    infile.open(to_string(i)+".txt");
    int count = 0;

    while(getline(infile, str)){
        count++;
        Transaction t;
        istringstream iss(str);
        iss >> t.id >> t.sale_amount >> t.name >> t.category;

        v.push_back(t);
    }

    mergesort(v, 0, count-1);

    char filename[20];

    snprintf(filename, sizeof(filename),"%d_%d.txt", i, append);

    FILE* in = openFile(filename, "w");

    for(int i = 0; i<count; i++){
        string temp = v[i].TrToString();
        fprintf(in, "%s \n", temp.c_str());
    }

    fclose(in);

}

// This is node of min-heap data structure
struct MinHeapNode{
    Transaction t;
    int file_no;
};

// Implementation of min-heap
class MinHeap {
    vector<MinHeapNode> harr;

public:
    MinHeap();

    int getSize(){
        return harr.size();
    }

    void MinHeapify(int);

    bool isEmpty(){
        if(harr.size() == 0)
            return true;
        else
            return false;
    }

    void push(MinHeapNode node);

    MinHeapNode top(){
        return harr[0];
    }

    void pop();
};

MinHeap::MinHeap() {

}

void MinHeap::MinHeapify(int i) {
    int leftChild = 2*i;
    int rightChild = 2*i + 1;
    int heap_size = getSize();
    int min = i;
    if(leftChild < heap_size && harr[leftChild].t.sale_amount < harr[i].t.sale_amount)
        min = leftChild;
    if(rightChild < heap_size && harr[rightChild].t.sale_amount < harr[min].t.sale_amount)
        min = rightChild;
    if(min != i){
        swap(harr[i], harr[min]);
        MinHeapify(min);
    }
}

void MinHeap::push(MinHeapNode node) {
    harr.push_back(node);

    int idx = harr.size() - 1;

    int parent = idx/2;

    while(idx > 0 && harr[idx].t.sale_amount < harr[parent].t.sale_amount){
        swap(harr[idx], harr[parent]);
        idx = parent;
        parent = parent/2;
    }
}

void MinHeap::pop(){
    int idx = harr.size()-1;
    swap(harr[0], harr[idx]);
    harr.pop_back();
    MinHeapify(0);
}


// sorting the (disk_block_size - 1) files at a time
void ExternalMergeSort(int start, int end, int run_size, int pass, int disk_block_size, vector<int> &arrPtr){
    cout<<"start = "<<start<<" "<<"end = "<<end<<endl;
    int k = (end-start+1);

    ifstream infile[k];
    char filename[5000];
    for(int j = start; j<=end; j += run_size){
        snprintf(filename, sizeof(filename), "%d_%d.txt", j, pass-1);
        infile[j-start].open(filename);
    }

    FILE* out[k];
    for(int j = start; j<=end; j++){
        snprintf(filename, sizeof(filename), "%d_%d.txt", j, pass);
        out[j-start] = openFile(filename, "w");
    }

    MinHeap minHeap;

    string str;
    for(int i = start; i<=end; i += run_size){
        if(getline(infile[i-start], str)){
            Transaction tt;
            istringstream iss(str);
            iss >> tt.id >> tt.sale_amount >> tt.name >> tt.category;
            MinHeapNode tempNode;
            tempNode.t = tt;
            tempNode.file_no = i;
            minHeap.push(tempNode);
        }
    }

    int out_file_no = 0, out_file_size = 0;
    while(!minHeap.isEmpty()) {
        MinHeapNode root = minHeap.top();

        string temp_str = root.t.TrToString();
        if(out_file_size >= disk_block_size){
           // fclose(out[out_file_no]);
            out_file_no++;
            out_file_size = 0;
        }

        fprintf(out[out_file_no], "%s \n", temp_str.c_str());
        out_file_size++;

        minHeap.pop();

        int temp_file_no = root.file_no-start;

  x:       if(getline(infile[temp_file_no], str)){
            Transaction tt;
            istringstream iss(str);
            iss >> tt.id >> tt.sale_amount >> tt.name >> tt.category;

            MinHeapNode tempNode;
            tempNode.t = tt;
            tempNode.file_no = temp_file_no + start;
            minHeap.push(tempNode);
            }else{
            if(arrPtr[root.file_no] != -1 && arrPtr[root.file_no] != root.file_no){
                snprintf(filename, sizeof(filename), "%d_%d.txt", root.file_no+1, pass-1);
                infile[root.file_no].close();
              //  infile[root.file_no+1 - start].open(filename);
                temp_file_no = root.file_no+1 - start;
                goto x;
            }
        }
    }

    for(int i = start; i<=end; i++){
        fclose(out[i-start]);
        infile[i-start].close();
    }



    for(int i = start; i<end; i++){
           arrPtr[i] = i+1;
    }
}

// Merging all files to output_file.txt when all the runs completed
void MergeAllFiles(int pass, int no_of_files){
    ifstream infile[no_of_files];
    char filename[5000];
    for(int j = 0; j<no_of_files; j += 1){
        snprintf(filename, sizeof(filename), "%d_%d.txt", j, pass);
        infile[j].open(filename);
    }

    FILE* out = openFile("output_file.txt", "w");
    string str;
    for(int i = 0; i<no_of_files; i++){
        while(getline(infile[i], str))
        {
            fprintf(out, "%s \n", str.c_str());
        }

        infile[i].close();
    }

    fclose(out);

}

int32_t main() {

// Number of records , disk block size, memory block size
    int records, disk_block_size, mem_block_size, no_of_disk_blocks;

    cin>>records>>disk_block_size>>mem_block_size;

    int pass;

    no_of_disk_blocks = records/disk_block_size;

    if(records % disk_block_size)
        no_of_disk_blocks++;


    // Generating random data for external merge sort

    vector<int> amount(records);

    //Randomly generated sales_amount
    srand((unsigned) time(0));
    for(int i = 0; i<records; i++){
        amount[i] = (rand() % 60000) + 1;
    }

    //Randomly generated names
    vector<string> names(records);
    srand((unsigned) time(0));
    for(int i = 0; i<records; i++){
        string str = "";

        for(int k = 0; k<3; k++){
            str += alphabet[rand() % MAX];
        }
        names[i] = str;
    }

    // Randomly generated categories
    vector<int> categories(records);
    srand((unsigned) time(0));
    for(int i = 0; i<records; i++){
        categories[i] = (rand() % 1500) + 1;
    }

    // Opening "input_file.txt" in writing mode
    FILE* in = openFile("input_file.txt", "w");

    for(int i = 0; i<records; i++){
        Transaction t;

        t.id = i+1;
        t.sale_amount = amount[i];
        t.name = names[i];
        t.category = categories[i];

        string temp = t.TrToString();

        fprintf(in, "%s \n", temp.c_str());
    }

      fclose(in);


    int no_of_files = records / disk_block_size;



    if(records % disk_block_size){
        no_of_files++;
    }
    pass = log_a_to_b(no_of_files,mem_block_size-1);

    divideFilesInDiskBlockSize("input_file.txt", disk_block_size, no_of_files);

    // This variable is to append to the filename - 0_0, 1_0, 2_0.....
    int append = 0;

    for(int i = 0; i<no_of_files; i++){
        sortIndividualFile(i, 0);
    }

    // This vector stores link to next file
    vector<int> arrPtr(no_of_files);

    for(int i = 0; i<no_of_files-1; i++){
        arrPtr[i] = i;
    }
    arrPtr[no_of_files-1] = -1;

    int total_files = no_of_files;

    // Initially individual file is sorted only
    int run_size = 1;;
    cout<<"pass ="<<pass<<endl;
    for(int i = 1; i<=pass; i++){
        int start = 0, end;
        int run = total_files/(mem_block_size-1);

        if(no_of_files % (mem_block_size-1)){
            run++;
        }

        for(int j = 1; j<=run; j++){
            if(total_files >= (mem_block_size-1)){
                total_files = total_files - (mem_block_size-1);
                end = start + run_size*(mem_block_size-1) - 1;
                cout<<"merging "<<start<<" to "<<end<<" files"<<endl;
                ExternalMergeSort(start, end, run_size, i, disk_block_size, arrPtr);
                start = end+1;
            }else{
                cout<<"merging "<<start<<" to "<<no_of_files-1<<" files"<<endl;
                ExternalMergeSort(start, no_of_files -1, run_size, i, disk_block_size, arrPtr);
                total_files = run;
            }
        }
        run_size *= (mem_block_size-1);
    }

    MergeAllFiles(pass, no_of_files);

    return 0;
}
