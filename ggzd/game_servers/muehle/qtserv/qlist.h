#ifndef QLIST_H
#define QLIST_H

#include <list>

template<class T> class QList : public list<T>
{
	public:
		QList(){}
		~QList(){m_list.clear();}
		void append(T* t){m_list.push_back(*t);}
		void setAutoDelete(bool del){m_autodelete=del;}
		T* first() {T* foo; *foo = m_list.front(); return foo;}
		T* next() {T* bar; *bar = m_list.back(); return bar;}
	private:
		list<T> m_list;
		bool m_autodelete;
};

#endif

