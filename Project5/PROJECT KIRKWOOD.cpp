#include <iostream>
#include <thread>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <iostream>
#include <string>
#include <complex>
#include <sstream>
#include <vector>
using namespace std;

volatile int tkp = 0;
volatile int stn = 0;// SATIR ve SUTUN DEÐÝÞKENÝ
int threadnumber = 100;
char xg = 'g';
int SIZE ;
ifstream file;
chrono::steady_clock sc;
int x, y, variable;
double  att;
double ptt;


template <class type>


class Matrix {

public:
	// standart yapýcý
	Matrix() { this->create(); }
	// özel yapýcý
	Matrix(int r, int c) { this->create(r, c); }
	// kopya yapýcý
	Matrix(const Matrix<type> &m) { this->copy(m); }
	// atama
	Matrix<type> operator=(const Matrix<type> &m);
	// yýkýcý
	~Matrix() { this->destroy(); }

	// eriþim fonklarý
	int getRows() const { return rows; }
	int getCols() const { return cols; }
	type* operator[](int i) const { return contents[i]; }

	// Matrix Operations
	Matrix<type> transpose() const;

	// Matrix çarpýmý
	friend Matrix<type> operator*(const Matrix<type> &m1, const Matrix<type> &m2)
	{
		Matrix<type> call; return call.multiply(m1, m2);
	}

	// matrix toplami
	friend Matrix<type> operator+(const Matrix<type> &m1, const Matrix<type> &m2)
	{
		Matrix<type> call; return call.addition(m1, m2);
	}

	// matrix 
	friend Matrix<type> operator-(const Matrix<type> &m1, const Matrix<type> &m2)
	{
		Matrix<type> call; return call.derogation(m1, m2);
	}


private:
	// fonksiyonlar
	void create();
	void create(int r, int c);
	void copy(const Matrix<type> &m);
	void destroy();

	// overload fonksiyon
	Matrix<type> multiply(const Matrix<type> &m1, const Matrix<type> &m2);
	Matrix<type> addition(const Matrix<type> &m1, const Matrix<type> &m2);
	Matrix<type> derogation(const Matrix<type> &m1, const Matrix<type> &m2);

	// üyüe deðiþkenler
	int rows;
	int cols;
	type** contents;

};


//yapýcý
template <class type>
void Matrix<type>::create() {
	rows = 0;
	cols = 0;
	contents = NULL;
}


// yapýcý
template <class type>
void Matrix<type>::create(int r, int c) {
	// Set Integer Values
	rows = r;
	cols = c;

	// Allocate Two-Dimensional Data
	contents = new type*[r];
	for (int i = 0; i < r; i++) {
		contents[i] = new type[c];
	}

}


// taklit yapýcý
template <class type>
void Matrix<type>::copy(const Matrix &m) {
	// Create New Matrix From Existing One
	this->rows = m.getRows();
	this->cols = m.getCols();

	// Allocate Two-Dimensional Data
	this->contents = new type*[m.getRows()];
	for (int i = 0; i < m.getRows(); i++) {
		this->contents[i] = new type[m.getCols()];
	}

	// Copy Over Data
	for (int i = 0; i < m.getRows(); i++) {
		for (int j = 0; j < m.getCols(); j++) {
			(*this)[i][j] = m[i][j];
		}
	}

}


//atama 
template <class type>
Matrix<type> Matrix<type>::operator=(const Matrix<type> &m) {
	// bir matrisi diðerinin üzerine yazar
	if (this != &m) {
		this->destroy();
		this->copy(m);
	}

	return *this;

}


// yokedici
template <class type>
void Matrix<type>::destroy() {
	//hafýzayý boþaltýr
	for (int i = 0; i < rows; i++) {
		delete[] contents[i];
	}
	delete[] contents;

}


// tersini alma
template <class type>
Matrix<type> Matrix<type>::transpose() const {

	Matrix<type> tran(cols, rows);

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			tran[j][i] = contents[i][j];
		}
	}

	return tran;

}


// paralel çarpým
template <class type>
void matrixParallel(const Matrix<type>* a, const Matrix<type>* b, Matrix<type>* c) {
	int BLM = a->getRows() / threadnumber;
	int NEWSTR = tkp;
	tkp = tkp + BLM;
	
	int BOYUT = a->getCols();
	int result = 0;

	//cout << endl  <<  "    NEWSTR=" << NEWSTR << "     tkp" << tkp;


	for (int i = NEWSTR; i < NEWSTR + BLM; i++) 
	{
		

		for (int j = 0; j < b->getCols(); j++) 
		{
			type result = 0;
			for (int k = 0; k < a->getCols(); k++) 
			{
				/*if (i>1022) {
					cout << endl << "i=" << i << "   k=" << k << "    j=" << j << "   BLM=" << BLM << "    NEWSTR=" << NEWSTR << "     str" << str;
					cout << endl << "a->getCols()" << a->getCols() << "     a->getRows()" << a->getRows() << "     b->getCols()" << b->getCols() << "     b->getRows()" << b->getRows();

				}*/
					result += ((*a)[i][k] * (*b)[k][j]);
				
				
				
			}
			(*c)[i][j] = result;
		}

	}

	



}


// çarpým
template <class type>
Matrix<type> Matrix<type>::multiply(const Matrix<type> &m1, const Matrix<type> &m2) {
	
	if (m1.getCols() != m2.getRows()) {
		cout << "Error: çarpým hatasý ";
		cout << "(" << m1.getRows() << "x" << m1.getCols() << ")*";
		cout << "(" << m2.getRows() << "x" << m2.getCols() << ")" << endl;
		return Matrix<type>();
	}


	// paralel

	Matrix<type> multiply(m1.getRows(), m2.getCols());

	int numCPU = threadnumber; // thread::hardware_concurrency(); kullanýlarak bilgisaayrdaki maksimum thread sayýsýda kullanýlabilir.
	thread* threads = new thread[numCPU];

	const Matrix<type>* m1Pointer = &m1;
	const Matrix<type>* m2Pointer = &m2;
	Matrix<type>* multiplyPointer = &multiply;

	//cout << endl;

	for (int i = 0; i < numCPU; i++) {

		threads[i] = thread(matrixParallel<type>, m1Pointer, m2Pointer, multiplyPointer);

	}

	for (int i = 0; i < numCPU; i++) {
		threads[i].join();
	}

	delete[] threads;

	return multiply;

}



///addition

template <class type>
Matrix<type> Matrix<type>::addition(const Matrix<type> &m1, const Matrix<type> &m2) {

	if (m1.getCols() != m2.getCols()) {
		cout << "Error: Toplama hatasý ";
		cout << "(" << m1.getRows() << "x" << m1.getCols() << ")*";
		cout << "(" << m2.getRows() << "x" << m2.getCols() << ")" << endl;
		return Matrix<type>();
	}

	if (m1.getRows() != m2.getRows()) {
		cout << "Error: Toplama hatasý ";
		cout << "(" << m1.getRows() << "x" << m1.getCols() << ")*";
		cout << "(" << m2.getRows() << "x" << m2.getCols() << ")" << endl;
		return Matrix<type>();
	}


	// paralel

	Matrix<type> multiply(m1.getRows(), m2.getCols());

	

	const Matrix<type>* m1Pointer = &m1;
	const Matrix<type>* m2Pointer = &m2;
	Matrix<type>* multiplyPointer = &multiply;

	
	for (int i = 0; i <m2Pointer->getRows(); i++)
	{
		
		for (int j = 0; j <m1Pointer->getCols(); j++)
		{
			
			(*multiplyPointer)[i][j] = ((*m1Pointer)[i][j] + (*m2Pointer)[i][j]);



		}
		 
	}

	


	

	

	return multiply;

}

template <class type>
Matrix<type> Matrix<type>::derogation(const Matrix<type> &m1, const Matrix<type> &m2) {

	if (m1.getCols() != m2.getCols()) {
		cout << "Error: Cikarma  hatasý ";
		cout << "(" << m1.getRows() << "x" << m1.getCols() << ")*";
		cout << "(" << m2.getRows() << "x" << m2.getCols() << ")" << endl;
		return Matrix<type>();
	}

	if (m1.getRows() != m2.getRows()) {
		cout << "Error: Cikarma hatasý ";
		cout << "(" << m1.getRows() << "x" << m1.getCols() << ")*";
		cout << "(" << m2.getRows() << "x" << m2.getCols() << ")" << endl;
		return Matrix<type>();
	}


	// paralel

	Matrix<type> multiply(m1.getRows(), m2.getCols());



	const Matrix<type>* m1Pointer = &m1;
	const Matrix<type>* m2Pointer = &m2;
	Matrix<type>* multiplyPointer = &multiply;


	for (int i = 0; i <m2Pointer->getRows(); i++)
	{

		for (int j = 0; j <m1Pointer->getCols(); j++)
		{

			(*multiplyPointer)[i][j] = ((*m1Pointer)[i][j] - (*m2Pointer)[i][j]);



		}

	}








	return multiply;

}



void main(int argc, char ** argv) {



	
	//system("PAUSE");
	

	cout << "**********************************************************************" << endl;
	cout << "**********************************************************************" << endl;
	cout << "Paralel matris carpma programina hos geldiniz.";
	cout << endl << "Bu program kullanicinin istedigi sayida thread kullanarak  her threadin isini bitirdiginde yeni bir is almasi esasina  "<<endl<<"dayanarak   paralel olarak   iki matrisi carpacaktir. ";
    cout << endl <<"Matris ve vektor dosyalari exe dosyasiyla ayni dizinde olmali. Yoksa hata alinir " << endl << "1024 carpim icin olmasi gereken  dosya adlari :b1k1k.txt ,e1k.txt ";
	cout << endl << "2048 carpim icin olmasi gereken  dosya adlari :b2k2k.txt ,e2k.txt" << endl;
	cout << endl << "Dosya adlari farkli ise program dosyalari okuyamayacagi icin gene hata alinir." << endl;

	cout << "**********************************************************************" << endl;
	cout << "**********************************************************************" << endl;

	cout << endl << "lütfen carpmak istediginiz matris boyutunu seciniz;";
	cout << endl << "a-)1024    b-)2048   " << endl;
	cin >> xg;

	while (true)
	{
		if (xg == 'a' || xg == 'b' )
		{
			if (xg == 'a') SIZE = 1024;
			if (xg == 'b') SIZE = 2048;
			
			break;
		}
		else
		{
			cout << endl << "Yanlis giris yaptiniz. Lutfen a veya b siklarindan birini seciniz" << endl;
			cin >> xg;
		}
	}

	xg = 'g';

	cout << endl<<"lutfen calismasini istediginiz thread sayisini seciniz;";
	cout << endl << "a-)1    b-)2    c-)4    d-)8" << endl;
	cin >> xg;

	while (true)
	{
		if (xg == 'a' || xg=='b' || xg=='c'  || xg=='d')
		{
			if (xg == 'a') threadnumber = 1;
			if (xg == 'b') threadnumber = 2;
			if (xg == 'c') threadnumber = 4;
			if (xg == 'd') threadnumber = 8;
			break;
		}
		else
		{
			cout <<endl<< "Yanlis giris yaptiniz. Lutfen a,b,c ve d siklarindan birini seciniz" << endl;
			cin >> xg;
		}
	}

	cout << endl  << threadnumber << " thread sayisi ile " << SIZE<<"lu matris carpimi programi calistiriliyor "<<endl<<"..........";
	cout << endl << "Matris okunmaya basliyor" << endl << "Matris dosyasinin okunmasi eski bilgisayarlarda ortalama olarak 60 sn zaman alicaktir" << endl << "lutfen bekleyin";
	cout << endl;
	
	///multiplacition 
	string str;
	string etcutera;
	string gecici = "";
	Matrix<double> left(SIZE, SIZE);
	Matrix<double> left1(SIZE, SIZE);
	Matrix<double> right(SIZE, 1);
	Matrix<double> right1(SIZE, 1);

	if (SIZE==1024)
	{
		

		/*for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
		left[i][j] = 1;
		left1[i][j] = 1;
		}
		}
		*/


		ifstream in("b1k1k.txt");

		if (!in) {
			cout << "Cannot open file.\n";

		}

		for (int ik = 0; ik < SIZE; ik++) {
			getline(in, gecici);
			/////

			string delims = ",";
			string policyStr = gecici;
			std::vector<std::string> results;
			size_t lastOffset = 0;

			while (true)
			{
				size_t offset = policyStr.find_first_of(delims, lastOffset);
				results.push_back(policyStr.substr(lastOffset, offset - lastOffset));
				if (offset == std::string::npos)
					break;
				else
					lastOffset = offset + 1; // add one to skip the delimiter
			}

			for (size_t i = 0; i < results.size(); i++)
			{

				////////
				etcutera = results[i];
				att = atof(etcutera.c_str());

				if (etcutera.find("i") != string::npos)
				{
					stringstream ss;
					ss << att;
					str = ss.str();
					str = str + "i";
					if (str == etcutera)
					{
						ptt = att;
						att = 0;
					}
					else
					{

						ptt = atof(etcutera.erase(0, str.length() - 1).c_str());
					}

				}
				else
				{

					ptt = 0;
				}

				left[ik][i] = att;//(i + 5 + 2 * k) % 12;
				left1[ik][i] = ptt;

				//std::cout << "the number is real " << att<<" sanal "<<ptt << "i\n";
				//std::cout << "dis dongu"<<ik << " ic dongu " << i << "   asil sonuc " << results[i] << std::endl;
				//////////////
			}

			//timer
			if (ik % 32 == 0) {
				cout << "*";
				if (ik / 32 == 0 || ik / 32 == 7 || ik / 32 == 10)
					cout << "  ";
				if (ik / 32 == 6 || ik / 32 == 20)
					cout << endl;
				if (ik / 32 == 9 || ik / 32 == 12)
					cout << "   " << endl;
				if (ik / 32 == 28)
					cout << endl << "   ";
				if (ik / 32 == 30)
					cout << "  ";
				if (ik / 32 == 31)
					cout << endl << "   **  *" << endl << "*****  *" << endl;

			}





		}


		in.close();
		cout << "Matris1BasariiLeokundu" << endl << "matris2 (vektor) okunmaya baslandi";



		//////

		gecici = "";


		

		/*for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j <1; j++) {
		right[i][j] = 1;
		right1[i][j] = 1;
		}
		}*/


		ifstream Kin("e1k.txt");

		if (!Kin) {
			cout << " Vektor Cannot open file.\n";

		}

		for (int ik = 0; ik < SIZE; ik++) {
			getline(Kin, gecici);
			/////

			string delims = "\n";
			string policyStr = gecici;
			std::vector<std::string> results;
			size_t lastOffset = 0;

			while (true)
			{
				size_t offset = policyStr.find_first_of(delims, lastOffset);
				results.push_back(policyStr.substr(lastOffset, offset - lastOffset));
				if (offset == std::string::npos)
					break;
				else
					lastOffset = offset + 1; // add one to skip the delimiter
			}

			for (size_t i = 0; i < results.size(); i++)
			{

				////////
				etcutera = results[i];
				att = atof(etcutera.c_str());

				if (etcutera.find("i") != string::npos)
				{
					stringstream ss;
					ss << att;
					str = ss.str();
					str = str + "i";
					if (str == etcutera)
					{
						ptt = att;
						att = 0;
					}
					else
					{

						ptt = atof(etcutera.erase(0, str.length() - 1).c_str());
					}

				}
				else
				{

					ptt = 0;
				}

				right[ik][i] = att;//(i + 5 + 2 * k) % 12;
				right1[ik][i] = ptt;

				//std::cout << "the number is real " << att<<" sanal "<<ptt << "i\n";
				//std::cout << "dis dongu"<<ik << " ic dongu " << i << "   asil sonuc " << results[i] << std::endl;
				//////////////
			}

			//timer
			if (ik % 32 == 0) {
				cout << "*";
				if (ik / 32 == 0 || ik / 32 == 7 || ik / 32 == 10)
					cout << "  ";
				if (ik / 32 == 6 || ik / 32 == 20)
					cout << endl;
				if (ik / 32 == 9 || ik / 32 == 12)
					cout << "   " << endl;
				if (ik / 32 == 28)
					cout << endl << "   ";
				if (ik / 32 == 30)
					cout << "  ";
				if (ik / 32 == 31)
					cout << endl << "   **  *" << endl << "*****  *" << endl;

			}





		}


		Kin.close();
		cout << "Matris2 (vektor) BasariiLeokundu" << endl;



	}
	else
	{

		

		/*for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
		left[i][j] = 1;
		left1[i][j] = 1;
		}
		}
		*/


		ifstream in("b2k2k.txt");

		if (!in) {
			cout << "Cannot open file.\n";

		}

		for (int ik = 0; ik < SIZE; ik++) {
			getline(in, gecici);
			/////

			string delims = ",";
			string policyStr = gecici;
			std::vector<std::string> results;
			size_t lastOffset = 0;

			while (true)
			{
				size_t offset = policyStr.find_first_of(delims, lastOffset);
				results.push_back(policyStr.substr(lastOffset, offset - lastOffset));
				if (offset == std::string::npos)
					break;
				else
					lastOffset = offset + 1; // add one to skip the delimiter
			}

			for (size_t i = 0; i < results.size(); i++)
			{

				////////
				etcutera = results[i];
				att = atof(etcutera.c_str());

				if (etcutera.find("i") != string::npos)
				{
					stringstream ss;
					ss << att;
					str = ss.str();
					str = str + "i";
					if (str == etcutera)
					{
						ptt = att;
						att = 0;
					}
					else
					{

						ptt = atof(etcutera.erase(0, str.length() - 1).c_str());
					}

				}
				else
				{

					ptt = 0;
				}

				left[ik][i] = att;//(i + 5 + 2 * k) % 12;
				left1[ik][i] = ptt;

				//std::cout << "the number is real " << att<<" sanal "<<ptt << "i\n";
				//std::cout << "dis dongu"<<ik << " ic dongu " << i << "   asil sonuc " << results[i] << std::endl;
				//////////////
			}

			//timer
			if (ik % 32 == 0) {
				cout << "*";
				if (ik / 32 == 0 || ik / 32 == 7 || ik / 32 == 10)
					cout << "  ";
				if (ik / 32 == 6 || ik / 32 == 20)
					cout << endl;
				if (ik / 32 == 9 || ik / 32 == 12)
					cout << "   " << endl;
				if (ik / 32 == 28)
					cout << endl << "   ";
				if (ik / 32 == 30)
					cout << "  ";
				if (ik / 32 == 31)
					cout << endl << "   **  *" << endl << "*****  *" << endl;

			}





		}


		in.close();
		cout << "Matris1BasariiLeokundu" << endl << "matris2 (vektor) okunmaya baslandi";



		//////

		gecici = "";


		

		/*for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j <1; j++) {
		right[i][j] = 1;
		right1[i][j] = 1;
		}
		}*/


		ifstream Kin("e2k.txt");

		if (!Kin) {
			cout << " Vektor Cannot open file.\n";

		}

		for (int ik = 0; ik < SIZE; ik++) {
			getline(Kin, gecici);
			/////

			string delims = "\n";
			string policyStr = gecici;
			std::vector<std::string> results;
			size_t lastOffset = 0;

			while (true)
			{
				size_t offset = policyStr.find_first_of(delims, lastOffset);
				results.push_back(policyStr.substr(lastOffset, offset - lastOffset));
				if (offset == std::string::npos)
					break;
				else
					lastOffset = offset + 1; // add one to skip the delimiter
			}

			for (size_t i = 0; i < results.size(); i++)
			{

				////////
				etcutera = results[i];
				att = atof(etcutera.c_str());

				if (etcutera.find("i") != string::npos)
				{
					stringstream ss;
					ss << att;
					str = ss.str();
					str = str + "i";
					if (str == etcutera)
					{
						ptt = att;
						att = 0;
					}
					else
					{

						ptt = atof(etcutera.erase(0, str.length() - 1).c_str());
					}

				}
				else
				{

					ptt = 0;
				}

				right[ik][i] = att;//(i + 5 + 2 * k) % 12;
				right1[ik][i] = ptt;

				//std::cout << "the number is real " << att<<" sanal "<<ptt << "i\n";
				//std::cout << "dis dongu"<<ik << " ic dongu " << i << "   asil sonuc " << results[i] << std::endl;
				//////////////
			}

			//timer
			if (ik % 32 == 0) {
				cout << "*";
				if (ik / 32 == 0 || ik / 32 == 7 || ik / 32 == 10)
					cout << "  ";
				if (ik / 32 == 6 || ik / 32 == 20)
					cout << endl;
				if (ik / 32 == 9 || ik / 32 == 12)
					cout << "   " << endl;
				if (ik / 32 == 28)
					cout << endl << "   ";
				if (ik / 32 == 30)
					cout << "  ";
				if (ik / 32 == 31)
					cout << endl << "   **  *" << endl << "*****  *" << endl;

			}





		}


		Kin.close();
		cout << "Matris2 (vektor) BasariiLeokundu" << endl;

	}
	


	

	


	//system("PAUSE");



	

	

	




	


	cout << endl << "Matrislerin dosyadan okunmasi tamamlandi. Carpma islemi basliyor...";
	 // create an object of `steady_clock` class
	auto start = sc.now();     // start timer
	cout << endl;

	Matrix<double> result1 = left * right;


	//system("PAUSE");

	tkp = 0;
	
	Matrix<double> result2 = left1 * right1;

	//cout << endl << left1[14][19];
	//cout << endl << right1[145][0];
	tkp = 0;
	Matrix<double> realresult = result1 - result2;
	//system("PAUSE");

	//cout << endl << realresult[145][0];
	Matrix<double> result3 = left1 * right;
	tkp = 0;
	//system("PAUSE");
	Matrix<double> result4 = left * right1;

	Matrix<double> sanalresult = result3 + result4;
	
	

	auto end = sc.now();       // end timer (starting & ending is done by measuring the time at the moment the process started & ended respectively)
	auto time_span = static_cast<chrono::duration<double>>(end - start);   // measure time span between start & end


	cout << SIZE<< "lu Matris carpmasi icin gecen toplam sure: " << time_span.count() << " saniye !!!" << endl;
	cout << endl << "Carpim sonucu sonuc.txt olarak kaydedilmeye baslandi bekleyiniz.....";



	ofstream myfile;
	myfile.open("sonuc.txt", ios::trunc);
	string force = "";
	string real;
	string imaginary;
	if (myfile.is_open()) 
	{
		for (int i = 0; i < SIZE;i++) 
		{
			/*stringstream kk;
			kk << realresult[i];
			real = kk.str();


			stringstream ccc;
			ccc << sanalresult[i];
			imaginary = ccc.str();
			imaginary = imaginary + "i";

			force = real + "   ,     " + imaginary;*/

			myfile << realresult[i][0];
			myfile << "  ,";
			myfile << sanalresult[i][0];
			myfile << " i";
			myfile <<"\n";
		}

		myfile.close();
		cout << endl << " sonuc.txt kaydedildi.program sonlandiriliyor";
	}
	else
	{
		cout << "HATA : sonuc.txt acilamadi. Kayit basarisiz ";
  
	}
	
	
	


	
	cout << endl << "....................";

	system("PAUSE");







	return ;
}

