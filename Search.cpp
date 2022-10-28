#include "Search.h"
#include <cstring>
using namespace std;

// REFERENCE: "Suffix Arrays: A New Method for On-Line String Searches" by Udi Manber and Gene Myers
//            https://courses.cs.washington.edu/courses/cse590q/00au/papers/manber-myers_soda90.pdf
//            "Suffix Array (Introduction)" by GeeksforGeeks
//            https://www.geeksforgeeks.org/suffix-array-set-1-introduction/
//
// Function implemented by Jade Tran and Nika Douri

namespace {

  // Helper function to find the start of the bucket
  // If "mid" is inside the correct range (bucket) and the bucket can be expanded to the left
  // Means the current mid is not the beginning of the bucket
  // Then sliding "mid" to the left by subtracting one until it reaches the beginning
  // Or patern[0] != text[result[new_left]]
  int find_start_bucket(const string& pattern,
                         const string& text,
                         const SuffixArray& result,
                         int mid) {
    int new_left = mid;
  
    while(new_left > 1 && pattern[0] == text[result[new_left-1]])
      new_left--;
    
    return new_left;
  }

  // Helper function to find the end of the bucket
  // If "mid" is inside the bucket and the bucket can be expanded to the right
  // Means the current mid is not the end of the bucket
  // Then moving "mid" to the right by adding one until it reaches the end
  // Or pattern[0] != text[result[new_right]]
  int find_end_bucket(const string& pattern,
                         const string& text,
                         const SuffixArray& result,
                         int mid) {
    int new_rigth = mid;
    
    while(new_rigth < (int)text.length() && pattern[0] == text[result[new_rigth+1]])
      new_rigth++;
    
    return new_rigth;
  }
}

// Search function applies Binary Search on Sorted Suffix Array to find the right bucket
// When the right bucket is found, takes O(nlogm), 
//      the program starts comparing to find all the occurences, takes O(z)
// Hence, the function has run time complexity is O(nlogm + z)
vector<size_t> searchFor(const string& pattern,
                         const string& text,
                         const SuffixArray& result) {
  
  // Vector array of all pattern's locations in text
  vector<size_t> locate;

  // If pattern is empty
  // The vectory array will contain [0, text.length()]
  if(pattern.length() == 0) {
    for (int i = 0; i <= (int)text.length(); i++) 
      locate.push_back(i);
  }
  else {

    // If pattern and text's lengths are equal -> just check if they are equal
    if(pattern.length() == text.length()) {   

      if(pattern == text)
        // If indeed they macth, then add 0 to the vector array
        locate.push_back(0);
    }
    else {

      int m = text.length();
      int n = pattern.length();
      int l = 0, r = m;   // Identify the range

      while (l <= r) {

        int mid = l + (r - l)/2;

        // If curently is not in the right bucket, then compare the first character
        // Case 1: pattern is alphabetically less than text at index mid
        //         move the range to the left [l, mid)
        if(pattern[0] < text[result[mid]]) {
          r = mid - 1;
        }
        // Case 2: pattern is alphabetically greater than text at index mid
        //         move the range to the right (mid, r]
        else if(pattern[0] > text[result[mid]]) {
          l = mid + 1;
        }
        else {

          // Find the start and end of the bucket according to the first character
          l = find_start_bucket(pattern, text, result, mid);  
          r = find_end_bucket(pattern, text, result, mid);

          // While it's in the valid range, start comparing to find the substring's occurences
          // Notice, as the suffix array is already sorted 
          // So, the found positions will always be in order
          while(l <= r) {

            // Compare pattern with substring of text (pattern.length() characters) start at index result[l]
            int position = strncmp(pattern.c_str(), text.c_str()+result[l], n);
            
            // If the comparison returns 0 mean they match, then add result[l] to the array
            if(position == 0)
              locate.push_back(result[l]);

            // Then moves to the next element in the bucket
            l++;
          } 
        }   
      }     
    }       
  } 

  // Return all locations - vector array
  return locate;

}
