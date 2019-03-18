#include <iostream>
#include <string>
using namespace std;

#include <map>
#include <math.h>
#include <vector>

#include <set>
#include <algorithm>
#include <functional>

//** Note: For the current problem, I did not find it useful to create a class since it is only one function, but if wanted more functionality for the cash register, I would create an instance of a class. ** //

int HUNDRED = 10000;
int FIFTY = 5000;
int TWENTY = 2000;
int TEN = 1000;
int FIVE = 500;
int TWO = 200;
int ONE = 100;
int HALF_DOLLAR = 50;
int QUARTER = 25;
int DIME = 10;
int NICKEL = 5;
int PENNY = 1;

using namespace std;

vector<string> delimit(string input, char search_str) {
  vector<string> ans;
  
  int delimit_pos = input.find(search_str); 
  ans[0] = input.substr(0, input.size() - delimit_pos); 
  ans[1] = input.substr(delimit_pos);
  
  cout << "in delimit before return";
  
  return ans;
}


int main(){
     //cost
  double x;
   //paid amt
  double y;
  int diff;
  
  double PP;
  double CH;
  
  
  string line;
  string firstVal;
  string secondVal;
  vector<string> vect;
  while (getline(cin, line)) {
    //vect = delimit(line, ';');
    int index = line.find(';');
     firstVal = line.substr(0,line.size() - index - 1);
     secondVal = line.substr(line.size() - index);
  }
  
  PP = stod(firstVal);
  CH = stod(secondVal);
  
  map<string, int> m;
  map<string, int>::iterator it;
  
  m["HUNDRED"] = 10000;
  m["FIFTY"] = 5000;
  m["TWENTY"] = 2000;
  m["TEN"] = 1000;
  m["FIVE"] = 500;
  m["TWO"] = 200;
  m["ONE"] = 100;
  m["HALF DOLLAR"] = 50;
  m["QUARTER"] = 25;
  m["DIME"] = 10;
  m["NICKEL"] = 5;
  m["PENNY"] = 1;


  // ** Note: A map will store in ascending order of key. For my implementation, I need to sort in decreasing order of value ** //
  // ** I wrote a comparator function that will reorder the map in decsending order of value ** //

  typedef function<bool(pair<string, int>, pair<string, int>)> Comparator;

  // Needed to write a lambda function to compare the two pairs, will compare pairs using 2nd field.

  Comparator compFunctor = 
  [](pair<string, int> elem1, pair<string, int> elem2){
  	return elem1.second > elem2.second;
  };

  //will then store the pairs using the logic above

  set<pair<string, int>, Comparator> s(m.begin(), m.end(), compFunctor);


//Logic for cash register

    x = PP;
    y = CH;
  
	x = x * 100;
	y = y * 100;

	diff = y - x;

	if (diff < 0)
	{
		cout << "ERROR" << endl;
        return 0;
	}

	if (diff == 0)
	{
		cout << "ZERO" << endl;
        return 0;
	}


for (pair<string, int> element : s){
  if(diff/element.second != 0){
    if(diff/element.second == 1){
      cout << element.first << ',';
    }else{
      cout << diff/element.second << element.first << ',';
    }
    
  }
    diff = fmod(diff, element.second);
}


return 0;
}
