/*
 * The Restriction class contains a function and a restriction type
 */

//less than
#define RESTRICTION_LESS	1 
//greater than
#define RESTRICTION_GREATER	2
//less or eq
#define RESTRICTION_LESSQ	3
//greater or eq
#define RESTRICTION_GREQ	4

class Restriction
{
private:
	//the restriction function
	Function frest;
	//restriction
	int restriction;
};