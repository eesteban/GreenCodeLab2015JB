#include <stdio.h>

/**
 *	This function replaces a character in a string with a specified pattern.
 */
int replace_char(char c, char* new_pattern, char* string, int str_len, int pattern_len) {
	char temp[1100];
	int temp_len = 0;
	for (int i = 0; i < str_len; i++) {
		if (string[i] != c) {
			temp[temp_len] = string[i];
			temp_len++;
			temp[temp_len] = '\0';
		} else {
			for (int j = 0; j < pattern_len; j++) {
				temp[temp_len] = new_pattern[j];
				temp_len++;
			}
			temp[temp_len] = '\0';
		}
	}
	for (int i = 0; i < temp_len; i++) {
		string[i] = temp[i];
	}
	return 0;
}

/**
 *	This function takes a string and its length and replaces the special characters
 *	that it contains (new line, tabulator) and replaces them by escape sequences;
 */
int replace_spec_chars(char* string, int str_len) {
	replace_char('\n',"#012",string,str_len,4);
	replace_char('\t',"#011",string,str_len,4);
	return 0;
}

/**
 *	The function takes two file names as arguments. It opens the first file,
 *	reads its content and writes it to the second file with special characters
 *	(new line, tabulator) replaced by escape sequences according to syslog.
 */
int replace_esc(char* in_file, char* out_file) {
	FILE* in;
	FILE* out;
	unsigned int c;
	char msg[1001];
	int msg_len;

	in = fopen(in_file,"r");
	out = fopen(out_file,"w+");
	msg[0] = '\0';
	c = fgetc(in);
	msg_len = 0;

	while (c != EOF) {
		while (c != '#') {
			msg[msg_len] = (unsigned char) c;
			msg_len++;
			msg[msg_len] = '\0';
			c = fgetc(in);
		}
		replace_spec_chars(msg,msg_len);
		fputs(msg,out);
		fputs("\n",out);
		fgetc(in);
		c = fgetc(in);
		msg_len = 0;
		msg[msg_len] = '\0';
	}

	fclose(in);
	fclose(out);
	return 0;	
}
