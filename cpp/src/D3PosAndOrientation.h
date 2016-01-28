/*
D3PosAndOrientation.h


www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license


History:
2015-03-09 Created by Henrik Bjorkman (www.eit.se/hb)

*/

#ifndef D3PAO_H
#define D3PAO_H



class WordReader;
class WordWriter;


class D3PosAndOrientation {

	public:






		// default constructor
		D3PosAndOrientation();

		// copy constructor
		D3PosAndOrientation(const D3PosAndOrientation &v);

		// assignment operator
		void operator=(const D3PosAndOrientation &v)
		{
			this->pos = v.pos;
			this->nose = v.nose;
			this->head = v.head;
		}

		// Destructor is not needed, there are no allocated resources.
		// ~D3Vector();

		// compare equals operator
		bool equals(const D3PosAndOrientation &v)
		{
			return (this->pos == v.pos) && (this->nose == v.nose) && (this->head == v.head);
		}

		void set(const D3PosAndOrientation &pao)
		{
			this->pos.set(pao.pos);
			this->nose.set(pao.nose);
			this->head.set(pao.head);
		}

		void set(const D3Vector &pos, const D3Vector &nose, const D3Vector &head)
		{
			this->pos.set(pos);
			this->nose.set(nose);
			this->head.set(head);
		}

		D3PosAndOrientation(const D3Vector &pos, const D3Vector &nose, const D3Vector &head);


		void normalizeToNose();
		void pitch_up(float rotationStep);
		void yaw_right(float rotationStep);
		void roll_right(float rotationStep);

		void forward(float distToMove);
		void right(float distToMove);
		void up(float distToMove);

		void readSelf(WordReader *wr);
		void writeSelf(WordWriter *ww);

		D3Vector pos;    // position of the object
		D3Vector nose;   // alias forward, direction in which objects nose it pointing
		D3Vector head;   // alias up, direction in which objects head it pointing (up) 



		void getInverseRotMatrix(float m[]) const;
		void MulMatrix4x4(const float m1[], const float m2[], float m3[]) const;

		void getTransMatrixToSuper(const D3Vector &pos, float m[]) const;
		void getRotMatrixToSuper(float m[]) const;
		void paoToMatrixToSuper(float m[]) const;
		void translatePaoToSuper(D3PosAndOrientation *pao) const;

		void getTransMatrixToLocal(const D3Vector &pos, float m[]) const;
		void getRotMatrixToLocal(float m[]) const;
		void paoToMatrixToLocal(float m[]) const;
		void translatePaoToLocal(D3PosAndOrientation *pao) const;


};

#endif

