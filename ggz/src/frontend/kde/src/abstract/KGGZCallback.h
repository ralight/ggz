#ifndef KGGZ_CALLBACK_H
#define KGGZ_CALLBACK_H

// Messaging Atoms
class KGGZCallback
{
	public:
		KGGZCallback(void* pointer, int opcode);
		~KGGZCallback();
		void* pointer();
		int opcode();
	private:
		void* m_pointer;
		int m_opcode;
};

#endif
