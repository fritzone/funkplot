/*
 * This class represents THE minimization problem. A correct problem contains:
 * 1. the definition of variables: x from -2 to 3 or x:-2..3
 * 2. the definiton of the function to minimize function f(x1,x2)=function in x1,x2
 * 3. the list of restrictions, they are also functions: restriction 4*x1-6*x2 <= 7
 *    A restriction contains:  a function: res1(x1,x2)=4*x1-6*x2
 *                             the type of restriction: '<=', etc...
 * 4. What we need: need: min f
 */

#include "util.h"
#include "stdio.h"

#include "HashTable.h"
#include "Function.h"
#include "List.h"
#include "Interval.h"

class Problem
{
private:
	//the function
	Function* f;
	//for each variable name there will be an interval type, defined later
	HashTable* vars;
	//the list of restrictions. In fact, list members are of type class Restriction
	List* restr;
	//the list of variables
	List* vardefs;

public:
	Problem()
	{
		vars=new HashTable();
		debug("we have a hashtable");
		restr=new List();
		vardefs=new List();
	}

	//this loads a problem from file: fn
	Problem(char *fn)
    {
        vars=new HashTable();
        debug("we have a hashtable");
        restr=new List();
        vardefs=new List();
        load(fn);
	}

	// this function loads the entire problem from a file, called fn
	void load(char* fn)
	{
		FILE* fp=fopen(fn,"rt");
		if(!fp)
		{
			error("No input file was found");
		}
		//now passing through the file
		while(!feof(fp))
		{
			char s[256];
			fgets(s,255,fp);
			//now we have to find out what the line is about:
			// 1. var [] means, we have a variable
			// 2. function [] means we hve a function
			// 3. restriction [] means, we have a restriction
			// 4. result [] means we ned that stuff
			int i=0;
			while(!isalpha(s[i++]));
			i--;
			char cmd[256];
			int cmdc=0;
			while(isalpha(s[i]))
			{
				cmd[cmdc++]=s[i++];
			}
			cmd[cmdc]=0;
			printf("%sM\n",cmd);
			//separators: only space+tab
			while(s[i]==32 || s[i]==9)
			{
				i++;
			}
			
			//now lets check waht are we supposed to read from the line
			//if we want to define a variable. syntax: var x from a to b. Separators: only spaces
			if(!strcmp(cmd,"var"))
			{
				
				char varname[30]; //think its pretty long :))
				int varnamec=0;
				while(isalnum(s[i]))
				{
					varname[varnamec++]=s[i++];
				}
				varname[varnamec]=0;
				//now lets skip the spaces
				while(s[i]==32 || s[i]==9)i++;
				//and the from
				while(s[i]!=32&&s[i]!=9)i++;
				//and the spaces;
				while(s[i]==32 || s[i]==9)i++;
				
				//the beginner of the interval
				char sa[10];
				int sac=0;
				while(isdigit(s[i])||s[i]=='.'||s[i]=='-')
				{
					sa[sac++]=s[i++];
				}
				sa[sac]=0;
				double a=atof(sa);
				printf("%.4f==%s must be %s\n",a,sa,varname);

				//skip the spaces;
				while(s[i]==32 || s[i]==9)i++;
				//and the to
				while(s[i]!=32&&s[i]!=9)i++;
				//and the spaces;
				while(s[i]==32 || s[i]==9)i++;
				//the ender of the interval
				char sb[10];
				int sbc=0;
				while(isdigit(s[i])||s[i]=='.'||s[i]=='-')
				{
					sb[sbc++]=s[i++];
				}
				sb[sbc]=0;
				double b=atof(sb);
				printf("%.4f==%s must be %s\n",b,sb,varname);
				Interval *intv=new Interval(a,b);
				char * vn=new char[strlen(varname)+1];
				strcpy(vn,varname);
				debug("mapping");
				vars->MapElement(vn,intv);
				printf("mapped a variable");

			}
		}
		fclose(fp);
		printf("Closed the file");
	}
};
