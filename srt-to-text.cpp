#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string.h>
#include <sstream>

using namespace std;

// Made Nov 25

/*
Update: Dec 13
Works for subtitles where the line for the text is empty (and so there is an extra line appended)

First line of the srt file must be a number, more specifically the index of the timestamp
*/


void readFile1(string filename, string outputfile) {
	int buffersize = 30;
	int linebuffersize = 150;
	char linebuffer[linebuffersize];
	char buffer[buffersize];
	ifstream infile;
	infile.open(filename);

    ofstream outfile; 
    outfile.open(outputfile);

    int numwords =0;
    int numlines=0;
    outfile << filename;

    int type = 0;
    int blocks=0;
    int blockcut=10;
    bool messedupline = false;

    string word;
    string prevtimestamp;
    char curtimestamp[64];

	while (!infile.eof()) {	
		infile.getline(linebuffer, linebuffersize);
	
		stringstream ss(linebuffer);
		if (type==0) {
			//printf("count = %s", linebuffer);
			type =2;
			continue;
		}

		else if (type==2) {
			ss >> word;

			// check difference between previous timestamp and current
			//printf("word.substr(5, 2) = %s\n", (word.substr(6, 2)).c_str() );

			float diff, prev_val_sec;
			if (!prevtimestamp.empty()) {
				//printf("prevword.substr(5, 2) = %s\n", (prevtimestamp.substr(6, 2)).c_str() );
				diff = (stoi(prevtimestamp.substr(6,2)) + ((float)stoi(prevtimestamp.substr(9,3)))/1000 ) - 
					(stoi(word.substr(6,2)) + ((float)stoi(word.substr(9,3))) /1000 );

				prev_val_sec = (stoi(prevtimestamp.substr(3,2))*60) + stoi(prevtimestamp.substr(6,2)) 
								+ (((float)stoi(prevtimestamp.substr(9,3))) /1000 );
				//printf("dif = %f\n\n", diff);
			}


			if ( (numlines % 50==0)&& !isspace(infile.peek()) ) {
				outfile << "\n\n";
				// Break up blocks of text into chunks to potentially aid readability, place to add a note
				if (blocks % blockcut ==0) {
					string barr1 = "////////////////////////////////////////////////////////////////////////////////////////////////\n";
					outfile << barr1 << barr1 << "\t" << blocks / blockcut 
						 << '\n' << barr1;	
				}
				outfile << '\n' << word << "\n\t";
				blocks++;
			}
			
			ss >> word;
			ss >> word;
			
			float cur_2_val_sec = (stoi(word.substr(3,2))*60) + stoi(word.substr(6,2))
								+ (((float)stoi(word.substr(9,3))) /1000 ); ;
			if (!(cur_2_val_sec==prev_val_sec)) {
				if ( (diff >= 3.3) || ( (diff<=-3.3)&&(diff>-10) ) ) {
					//printf("big dif\n");
					outfile << "\n\n\t";
				}

				if (!isspace(infile.peek())) prevtimestamp = word;
			}
			
			type =3;
			continue;
		}
		else if (type==3) {
			type =4;
			if (string(linebuffer).empty()) {
				messedupline = true;
				continue;
			}

		}
		else if (type==4) {
			// newline

			if (messedupline) {
				infile.getline(linebuffer, linebuffersize);
				messedupline = false;
			}
			type =0;
			continue;
		}

		string word;




		while (ss >> word) {
			if (word.empty()) {
				break;
			}


			if ( (word.compare("um")==0)||(word.compare("uh")==0)) {
				word = "..";
			}
			outfile << word << ' ';
			numwords++;
		}
		numlines++;
		
	}
	outfile << '\n';

	infile.close();
    outfile.close();
}

int main (int argc, char ** const argv) {
	if (argc!=3) {
		printf("Usage: ./a.out input output\n");
		return -1;
	}

	string filename, filename2;
	filename = argv[1];
	filename2 = argv[2];
	readFile1(filename, filename2);
}


