#include <StringExt>

StringExt::StringExt()
: string()
{
}

StringExt::~StringExt()
{
}

list<string> StringExt::explode(string delimiter)
{
	string moo;
	string foo;
	int finder;
	list<string> bing;

	foo = this->data();
	moo = foo;

	bing.clear();
	while((!moo.empty()) && (finder != -1))
	{
		finder = foo.find(delimiter);
		moo = foo.substr(0, finder);
		foo = foo.substr(finder + 1, foo.length() - finder - 1);
		bing.push_back(moo);
	}
	return bing;
}

string StringExt::operator=(const char assignment[])
{
	this->erase();
	this->append(assignment);
}
