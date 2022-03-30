/*
 * The interval type
 */
class Interval
{
private:
	double begin;
	double end;
public:
	Interval()
	{
	}

	//this constructs an interval.
	Interval(double a, double b)
	{
		begin=a;
		end=b;
	}

};