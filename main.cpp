/*
 Sorting toolkit - basic tools to build and measure sorting algorithms.
 */
#include <iostream>
#include <iomanip>
#include <random>
#include <string>
#include <cmath>
#include <assert.h>
#include <string>
// using namespace std; // avoid confusion between ::swap and std::swap
using std::cout;
using std::setw;
using std::endl;
using std::string;
using std::vector;

static const int SCREEN_WIDTH=80;
#define DEBUG 0

template <typename T>
void show(T[], size_t, int screen_width=SCREEN_WIDTH);

unsigned long long rdtsc() { // intel / AMD clock ticks, Read Time-Stamp Counter
	unsigned int low, high;
	__asm__ __volatile__("rdtsc" : "=a" (low), "=d" (high));
	return ((unsigned long long)high << 32) | low;
}

template <typename T>
void swap (T &left, T &right) {
	T temp=left; left=right; right=temp;
}

template <typename T>
size_t index_of_min(T array[], size_t size) {
	assert(size>0);
	size_t smallest_index=0;
	T smallest=array[smallest_index];
	for (size_t i=1; i<size; ++i) {
		if (array[i]<smallest) {
			smallest=array[i];
			smallest_index=i;
		}
	}
	return smallest_index;
}

template <typename T>
size_t index_of_max(T array[], size_t size) {
	assert(size>0);
	size_t largest_index=0;
	T largest=array[largest_index];
	for (size_t i=1; i<size; ++i) {
		if (array[i]>largest) {
			largest=array[i];
			largest_index=i;
		}
	}
	return largest_index;
}

template <typename T>
void selection_sort(T array[], size_t size) {
	for(size_t max_right=size-1; max_right>0 ; --max_right) {
		size_t max_i=index_of_max(array, max_right+1);
		if (DEBUG and size<20) {
			cout<<"swap array["<<max_i<<"]="<<array[max_i]<<" with "
			<<"array["<<max_right<<"]="<<array[max_right]<<endl;
			show(array, size);
			cout<<endl;
		}
		swap(array[max_i], array[max_right]);
	}
}

template <typename T>
void selection_sort_1a(T array[], size_t size) {
	for(size_t max_right=size-1; max_right>0 ; --max_right) {
		size_t max_i=index_of_max(array, max_right+1);
		if (DEBUG and size<20) {
			cout<<"swap array["<<max_i<<"]="<<array[max_i]<<" with "
			<<"array["<<max_right<<"]="<<array[max_right]<<endl;
			show(array, size);
			cout<<endl;
		}
		swap(array[max_i], array[max_right]);
	}
}
// for whatever reason this is actually slower due to the function call
// of index_of_max and index_of_min
// could combine them into a single function and speed it up?
// where is it bottlenecking?
template <typename T>
void selection_sort_1b(T array[], size_t size) {
	size_t min_left=0;
	for(size_t max_right=size-1; max_right>0 ; --max_right, min_left++) {
		size_t max_i=index_of_max(array, max_right+1);
		size_t min_i=index_of_min(array, max_right+1);
		if (DEBUG and size<20) {
			cout<<"swap array["<<max_i<<"]="<<array[max_i]<<" with "
			<<"array["<<max_right<<"]="<<array[max_right]<<endl;
			show(array, size);
			cout<<endl;
		}
		swap(array[max_i], array[max_right]);
		swap(array[min_i], array[min_left]);
	}
}
template <typename T>
bool is_sorted_ascending(T array[], size_t size) {
	// returns true if array is in ascending sorted order, else false.
	for (size_t i=0; i<(size-1); ++i)
		if (array[i]>array[i+1]) {
			return false;
		}
	return true;
}

enum data_property_t {increasing, decreasing, RANDOM, same, property_terminator};
template <typename T>
void fill(T array[], size_t size, data_property_t property=RANDOM) {
	assert(size>0);
	static bool seeded=false;
	if (!seeded) {
		srand(time(0));
		seeded=true;
	}
	switch (property) {
		case decreasing:
			for (size_t i=0; i<size; ++i)
				array[i]=(size-1)-i;
			break;
		case increasing:
			for (size_t i=0; i<size; ++i)
				array[i]=i;
			break;
		case RANDOM:
			for (size_t i=0; i<size; ++i)
				array[i]=rand()%size;
			break;
		case same:
			T same_element_value=rand()%size;
			for (size_t i=0; i<size; ++i)
				array[i]=same_element_value;
			break;
	}
}

template <typename T>
void show(T array[], size_t size, int screen_width) {
	// display all elements of array, separated into columns and lines
	// begin each new line with the current array offset
	assert(size>0);
	int width_per_element=log10(size)+2;
	int elements_per_line=screen_width/width_per_element;
	bool start_new_line=true;
	for (size_t i=0; i<size; ++i) { // for each element in the array
		if (start_new_line) {
			cout<<setw(width_per_element)<<i<<":"; // show index at start of each line
			start_new_line=false;
		}
		cout<<setw(width_per_element)<<array[i]; // display the element
		if (i%elements_per_line==(elements_per_line-1)) { // line break needed?
			cout<<endl; // provide line break
			start_new_line=true;
		}
	}
	if (!start_new_line) cout << endl;
	// show size, location and value of min, max in array
	int min_i=index_of_min(array, size), max_i=index_of_max(array, size);
	cout<<"size="<<size<<" min["<<min_i<<"]="<<array[min_i]
	<<" max["<<max_i<<"]="<<array[max_i]<<endl<<endl;
}

void test_arrays(int init_size=10, int multiplier=10, int repetitions=3) {
	// by default, fill and show arrays of sizes: 10, 100, 1000
	for (int rep=0, size=init_size; rep<repetitions; ++rep, size*=multiplier) {
		int array[size];
		data_property_t property=static_cast<data_property_t>(rep%property_terminator);
		fill(array, size, property);
		show(array, size);
	}
}

template <typename T>
long long time_sort(void sort(T [], size_t), T array[], T size) {
	auto rdtsc_clocks_start=rdtsc();
	sort(array, size);
	auto rdtsc_clocks_stop=rdtsc();
	auto duration=rdtsc_clocks_stop-rdtsc_clocks_start;
	return duration;
}

void test_selection_sort(int init_size=10, int multiplier=2, int repetitions=5,
						 data_property_t property=RANDOM) {
	// by default, try sorting algorithm on array sizes: 10, 20, 40, 80, 160
	for (int rep=0, size=init_size; rep<repetitions; ++rep, size*=multiplier) {
		int array[size];
		fill(array, size, property);
		show(array, size); // show initial unsorted array
		selection_sort(array, size);
		if (!is_sorted_ascending(array, size)) {
			cout<<"Oops! array is not sorted!\n";
		}
		show(array, size); // show final sorted array
	}
}
void test_selection_sort1a(int init_size=10, int multiplier=2, int repetitions=5,
						  data_property_t property=RANDOM) {
	// by default, try sorting algorithm on array sizes: 10, 20, 40, 80, 160
	for (int rep=0, size=init_size; rep<repetitions; ++rep, size*=multiplier) {
		int array[size];
		fill(array, size, property);
		show(array, size); // show initial unsorted array
		selection_sort_1a(array, size);
		if (!is_sorted_ascending(array, size)) {
			cout<<"Oops! array is not sorted!\n";
		}
		show(array, size); // show final sorted array
	}
}

void test_selection_sort1b(int init_size=10, int multiplier=2, int repetitions=5,
						   data_property_t property=RANDOM) {
	// by default, try sorting algorithm on array sizes: 10, 20, 40, 80, 160
	for (int rep=0, size=init_size; rep<repetitions; ++rep, size*=multiplier) {
		int array[size];
		fill(array, size, property);
		show(array, size); // show initial unsorted array
		selection_sort_1b(array, size);
		if (!is_sorted_ascending(array, size)) {
			cout<<"Oops! array is not sorted!\n";
		}
		show(array, size); // show final sorted array
	}
}
vector<long long> time_selection_sort
(int init_size=10, int multiplier=2, int repetitions=5, data_property_t property=RANDOM) {
	// by default, time sorting algorithm on array sizes: 10, 20, 40, 80, 160
	vector<long long> results;
	for (int rep=0, size=init_size; rep<repetitions; ++rep, size*=multiplier) {
		int array[size];
		fill(array, size, property);
		auto duration=time_sort(selection_sort, array, size);
		results.push_back(size);
		results.push_back(duration);
		if (!is_sorted_ascending(array, size)) {
			cout<<"Oops! array is not sorted!\n";
		}
	}
	return results;
}

vector<long long> time_selection_sort_1b
(int init_size=10, int multiplier=2, int repetitions=5, data_property_t property=RANDOM) {
	// by default, time sorting algorithm on array sizes: 10, 20, 40, 80, 160
	vector<long long> results;
	for (int rep=0, size=init_size; rep<repetitions; ++rep, size*=multiplier) {
		int array[size];
		fill(array, size, property);
		auto duration=time_sort(selection_sort_1b, array, size);
		results.push_back(size);
		results.push_back(duration);
		if (!is_sorted_ascending(array, size)) {
			cout<<"Oops! array is not sorted!\n";
		}
	}
	return results;
}

vector<long long> time_selection_sort_1a
(int init_size=10, int multiplier=2, int repetitions=5, data_property_t property=RANDOM) {
	// by default, time sorting algorithm on array sizes: 10, 20, 40, 80, 160
	vector<long long> results;
	for (int rep=0, size=init_size; rep<repetitions; ++rep, size*=multiplier) {
		int array[size];
		fill(array, size, property);
		auto duration=time_sort(selection_sort_1a, array, size);
		results.push_back(size);
		results.push_back(duration);
		if (!is_sorted_ascending(array, size)) {
			cout<<"Oops! array is not sorted!\n";
		}
	}
	return results;
}

void display_timed_selection_sorts() {
	vector<long long> results = time_selection_sort(10, 2, 5, RANDOM);
	cout<<"Performance results for: selection_sort on random:\n";
	for (int rep=0; rep<results.size(); rep+=2) {
		cout<<"rep="<<setw(2)<<rep/2+1<<" size="<<setw(5)
		<<results[rep]<<" duration="<<results[rep+1]<<endl;
	}
}

template <typename T>
int partition(T array[], int first, int last) {
	T pivot=array[first]; // HERE IS WHERE THE ESTIMATED MEDIAN IS CHOSEN
						  // Using array[first], the task of picking a good estimated median is neglected
						  // If the array is increasing or decreasing, this is a very poor choice.
	int left=first, right=last;
	while (left<right) {
		while (pivot<array[right]) --right;
		while (left<right and array[left]<=pivot) ++left;
		if (left<right) swap(array[left], array[right]);
	}
	swap(array[first], array[right]);
	return right;
}

template <typename T>
void quicksort_worker(T array[], int first, int last) {
	int pos=0; // position of estimated median
	if (first<last) {
		pos=partition(array, first, last);
		quicksort_worker(array, first, pos-1);
		quicksort_worker(array, pos+1, last);
	}
}

template <typename T>
void quicksort(T array[], int size) {
	// this is a call adapter; quicksort_worker wants (array[], first, last);
	quicksort_worker(array, 0, size-1);
}

void test_quicksort(int init_size=10, int multiplier=2, int repetitions=5,
					data_property_t property=RANDOM) {
	// by default, try sorting algorithm on array sizes: 10, 20, 40, 80, 160
	for (int rep=0, size=init_size; rep<repetitions; ++rep, size*=multiplier) {
		int array[size];
		fill(array, size, property);
		show(array, size); // show initial unsorted array
		quicksort(array, size);
		if (!is_sorted_ascending(array, size)) {
			cout<<"Oops! array is not sorted!\n";
		}
		show(array, size); // show final sorted array
	}
}


int main(int argc, char** argv) {
	cout<<"start...\n";
	
	// test_arrays(5, 2, 5);
	test_selection_sort();
	test_selection_sort1a();
	// test_selection_sort(10, 2, 5, decreasing);
	// test_quicksort(10, 2, 5, random);
	display_timed_selection_sorts();
	
	cout<<"\n...stop\n";
	return 0;
}

/*
 
 */
