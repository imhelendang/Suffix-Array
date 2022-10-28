#include "SAIS.h"
#include "DC3.h"
#include <vector>
#include <set>
#include <algorithm>
#include <iostream>
#include <map>
using namespace std;

namespace {

  // Helper function
  // Preprocess the text[] to find number of unique characters - O(n)
  // Reference: https://www.javatpoint.com/cpp-algorithm-unique-function#:~:text=C++%20Algorithm%20unique%20()%20function%20is%20used%20to,second%20version%20uses%20the%20given%20binary%20predicate%20pred.
  vector<int> unique(const vector<size_t>& text, int n) {

    vector<int> result;
    int cnt = 0;

    vector<int> v;
    for(int i = 0; i < (int)n; i++)
      v.push_back((int)text[i]);
      
    sort(v.begin(), v.end());   
    auto last = unique(v.begin(), v.end());  
    v.erase(last, v.end());

    // Find how many time each unique number occurs in the array
    for(int i : v) {
      result.push_back(i);
      cnt = count(text.begin(), text.end(), i);
      result.push_back(cnt);
    }

    return result; 
  }

  // Step 1 - Annotate each charatcer and find LMS character
  void annotate(const vector<size_t>& text, vector<char>& sl, vector<int>& lms) {

    int j = 1;
    int n = (int)text.size();
    char l_type = 'L';

    // Note sentinel is defined to be S-type
    for (int i = n-2; i >= 0; i--) {

      // Base case
      if(i == n-2) {
        sl[i] = l_type;
      }
      else {

        // First case: if the text at i is greater than text at i+1
        //             then assign sl[i] to L
        if(text[i] > text[i+1]) {
          sl[i] = l_type;

          // Update lms array
          if(sl[i+1] == 'S') {
            lms.push_back(i+1);
            j++;
          }
        }

        // Second case: text[i] is equal to text[i+1], and sl[i+1] is l_type
        //              then assign sl[i] to L
        else if(text[i] == text[i+1] && sl[i+1] == l_type) {
          sl[i] = l_type;

        } // end else-if
      }   // end else
    }     // end for loop
  }       // end function

  // Helper function - Pass 1
  vector<int> pass1(const vector<size_t>& text, const vector<int>& lms, int character, int num) {
    
    vector<int> temp(num, 0);
    int i = num - 1;
    for(int j = 1; j < (int)lms.size(); j++) {
      if((int)text[lms[j]] == character) {
        temp[i] = lms[j];
        i--;
      }
    }
    return temp;
  }

  // Helper function - Pass function
  // Add the index to the correct position in the bucket
  // Run time: 
  // Best time: O(1)
  // Average time: O(bucket.size())
  // Worst time: O(bucket.size())
  void pass(SuffixArray& result, int num_pass, int start, int end, int add, int condi) {
    
    // Pass phrase 2
    if(num_pass == 2) {
      for(int i = start; i <= end; i++) {
        if((int)result[i] == condi) {
          result[i] = add;
          break;
        }
      }
      return;
    }
    // Pass phrase 3
    else {
      for(int i = end; i >= start; i--) {
        if((int)result[i] == condi) {
          result[i] = add;
          break;
        }
      }
      return;
    }
  }

  // Step 2 - Implement Induced Sorting
  SuffixArray sorted(const vector<size_t>& text,
                     const vector<char>& sl,
                     const vector<int>& lms,
                     const vector<int> num,
                     vector<int>& start,
                     vector<int>& end) {
    
    SuffixArray result((int)text.size(), 0);
    result[0] = lms[0]; // Sentinel always is in the beginning of result[]

    // Pass 1
    // A reverse pass over the input array of LMS suffixes, 
    // placing each one into the next free slot at the end of its bucket. 
    int i = 0, index = 0;
    while(i < (int)num.size()) {
      
      // Find start bucket
      start.push_back(index);

      vector<int> temp = pass1(text, lms, num[i], num[i+1]);

      for(int x = 0; x < num[i+1]; x++) {
        result[index] = temp[x];
        index++;
      }
      
      // Find end bucket
      end.push_back(index-1);
      // Move to the next pair
      i += 2;
    }
    result[0] = lms[0]; // Sentinel always is in the beginning of result[]
    
    // Pass 2
    // A forward pass over the suffix array, 
    // finding L-type suffixes and placing them into the first free slot at the front of their buckets. 
    for(int a = 0; a <= (int)result.size() - 1; a++) {
      if(sl[result[a] - 1] == 'L') {
        i = 0;
        // Check which bucket
        while((int)text[result[a] - 1] != num[i] && i <= (int)num.size()-2)
          i += 2;

        pass(result, 2, start[i/2], end[i/2], result[a] - 1, 0);
      }
    }
    
    // Pass 3 
    // A reverse pass over the suffix array,
    // finding S-type suffixes and placing them into the first free slot at the end of their buckets. 
    // Step 1: Reset indices
    for(int a = 0; a < (int)result.size() - 1; a++) 
      if(sl[result[a]] != 'L')
        result[a] = -1;
    
    // Now reverse
    //result[0] = result.size() - 1;    // Sentinel is always in the beginning
    for(int a = (int)result.size() - 1; a >= 1; a--) {
      
      if(sl[result[a] - 1] != 'L') {
        i = 0;
        // Check which bucket
        while((int)text[result[a] - 1] != num[i] && i <= (int)num.size()-2)
          i += 2;
      
        pass(result, 3, start[i/2], end[i/2], result[a] - 1, -1);
      }
    }
    result[0] = lms[0]; // Sentinel always is in the beginning of result[]
    return result;
  }

  // Helper function to sort new lms[] and reduced[]
  // REFERENCE: DelftStack
  // Link: https://www.delftstack.com/howto/cpp/vector-pair-cpp/
  void sort_lms(vector<int>& lms, vector<size_t>& reduced) {

    int n = (int)lms.size();
    vector<int> temp = lms;

    // Combine 2 corresponding array to sort
    vector<pair<int, size_t>> pairs;

    for(int i = 0; i < n; ++i)
      pairs.push_back(make_pair(temp[i], reduced[i]));

    // Sort the pairs
    sort(pairs.begin(), pairs.end());

    // Update reduced[] after sort()
    for(int i = 0; i < n; ++i) {
      lms[i] = pairs[i].first;
      reduced[i] = pairs[i].second;
    }
    
    return;
  }

  // Helper function to update lms[]
  void update_lms(const vector<size_t>& result,
                  vector<int>& lms) {
    
    vector<int> temp = lms;
    int j = 0;
    for(int i = 0; i < (int)result.size(); i++) {
      int x = 0;
      while(j < (int)temp.size() && x < (int)temp.size()) {
        if(result[i] == temp[x]) {
          lms[j] = result[i];
          j++;
        }
        x++;
      }
    }
    return;
  }


} // end namespace

SuffixArray sais(const vector<size_t>& text) {
  
  // Parallel vector arrays to find start and end of each bucket
  vector<int> start, end;   

  // Preprocess text[]
  vector<int> num = unique(text, (int)text.size());

  // Vector array to store S or L type corresponding to text[]
  // Fill the array with S - base case
  vector<char> sl((int)text.size(), 'S');  

  // Vector array to store indices of lms characters within the string
  // Sentinel always is LMS character (base case)
  vector<int> lms; 
  lms.push_back((int)text.size()-1);
  
  // Step 1:
  annotate(text, sl, lms);

  // Step 2:
  SuffixArray result = sorted(text, sl, lms, num, start, end);
  update_lms(result, lms);
  
  // Step 3:
  vector<size_t> reduced((int)lms.size(), 0);
  int i = 0;
  while(i < (int)lms.size() - 1) {
  
    reduced[i] = i;

    // Check if they are identical
    if(i > 0) {
      int x = min(lms[i], lms[i-1]);
      int y = max(lms[i], lms[i-1]);
      
      // If they have same length
      if(x == (y-x)) {

        int j = 0;
        // Check if all characters match
        while(text[lms[i] + j] == text[lms[i - 1] + j])
          j++;
        // When there are no characters left to compare from both side
        // And all char match -> they are identical
        // Then they have same reduced value
        if(j+1 == x && j+1 == y)
          reduced[i] = reduced[i-1];
      }
    }
    i++;
  }
  sort_lms(lms, reduced);

  // Step 4: 
  SuffixArray reduced_string = dc3(reduced);
  
  // Sort the LMS substrings
  for(int i = 0; i < (int)reduced_string.size(); i++) {
    reduced[i] = lms[reduced_string[i]];
  }

  // Step 5:
  // Step 1: Reset indices
  for(int a = 0; a < (int)result.size(); a++) 
    result[a] = -1;

  // Insert sorted indices
  // Pass 1
  int j = 0;
  for(int i = (int)lms.size() - 1; i >= 0; i--) {
    if((int)reduced[i] == (int)result.size() - 1) {
      result[0] = reduced[i];
    }
    else {
      j = 0;
      // Check which bucket
      while((int)text[reduced[i]] != num[j] && j < (int)num.size()/2)
        j += 2;

      pass(result, 3, start[j/2], end[j/2], reduced[i], -1);
    }
  }

  // Pass 2
  for(int a = 0; a < (int)result.size(); a++) {
    if(sl[result[a] - 1] == 'L') {
      j = 0;
      // Check which bucket
      while((int)text[result[a] - 1] != num[j] && j <= (int)num.size()-2)
        j += 2;

      pass(result, 2, start[j/2], end[j/2], result[a] - 1, -1);
    }
  }

  // Pass 3
  // Reset indices
  for(int a = 0; a < (int)result.size(); a++) 
    if(sl[result[a]] != 'L')
      result[a] = -1;
  
  
  // Now reverse
  result[0] = (int)result.size() - 1;    // Sentinel is always in the beginning
  for(int a = (int)result.size() - 1; a >= 1; a--) {
    if(sl[result[a] - 1] != 'L') {
      j = 0;
      // Check which bucket
      while((int)text[result[a] - 1] != num[j] && j < (int)num.size()/2)
        j += 2;
      
      pass(result, 3, start[j/2], end[j/2], result[a] - 1, -1);
    }
  }
  return result;
}

