#include "LZ77.h"
#include <iostream>
using namespace std;

LZ77::LZ77(Image *image)
{
	this->image = image;
	this->s_buff_size=16;//(changing require to change code //size of search buffer
	this->la_buff_size=20;				      //size of lookahead buffer
	if (this->la_buff_size<10)
	{
		cout<<"Error- too small size of lookahead buffer";
		exit(0);
	}	
}

LZ77::~LZ77()
{

}

void LZ77::encode()
{
    //pozniej sie podzieli na funkcje, teraz latwiej mi pisac ciagiem
	//pozniej posprzatam, zakomentowane rzeczy odkomentowuje do testow

	std::ofstream ofile("LZ77", std::ios::binary);
	unsigned int i, it=0, it2=0;			//iterators
	unsigned int x=1, y=0; 					//coordinates of pixels
	int length=0, max, cpmax=0, position=0; //variables to search the longest sequence
	bool end_of_picture=0, if_max_length_stop=0;
	SDL_Color clr=this->image->getPixelColorRGB(0,0);
	short what_color=1;						//which subpixel
	Uint8 color[3], code;
	color[0]=clr.r>>4;
	code=color[0];
	ofile<<code;							//the first color goes to file	
	color[1]=clr.g>>4;
	color[2]=clr.b>>4;			
	map<int, int>::iterator la_begin;
	map<int, int>::iterator s_begin;		
	map<int, int>::iterator count;
	map<int, int>::iterator temp;
	//map<int, int>::iterator e;
	int petla=-1;//do testow, pozniej usune

	//initialization of search buffer 	
	for (i=0; i<s_buff_size; ++i)
		s_buff.insert(pair<int, int>(i,color[0]));

	map<int, int>::iterator s_end=s_buff.end();
	
	//the first loading data to la_buff 
	while (la_buff.size()<la_buff_size && end_of_picture==0)
	{
		if (what_color<3)
			la_buff.insert(pair<int, int>(it2++,color[what_color++]));
		else
		{	
			if (x>=image->width())
			{
				x=0;
				++y;
				if (y>=image->height())
				{	
					end_of_picture=1;
					break;
				}
			}
			clr=this->image->getPixelColorRGB(x++,y);
			what_color=1;
			color[0]=clr.r>>4;
			color[1]=clr.g>>4;
			color[2]=clr.b>>4;	
			la_buff.insert(pair<int, int>(it2++,color[0]));	
		}
	}

	while(la_buff.size()>0 && end_of_picture==0)
	{
		petla++;	

		//loading data to la_buff 
		it2=it-cpmax;
		//if (petla<327)
		while (la_buff.size()<la_buff_size && end_of_picture==0)
		{
			//cout<<"x: "<<x<<endl;
			//cout<<"y: "<<y<<endl;
			if (what_color<3)	
				la_buff.insert(pair<int, int>(la_buff_size+1+it2++,color[what_color++]));	
			else
			{	
				if (x>=image->width())
				{
					x=0;
					++y;
					if (y>=image->height())
					{	
						end_of_picture=1;
						break;
					}
				}
				clr=this->image->getPixelColorRGB(x++,y);
				what_color=1;
				color[0]=clr.r>>4;
				color[1]=clr.g>>4;
				color[2]=clr.b>>4;
				la_buff.insert(pair<int, int>(la_buff_size+1+it2++,color[0]));		
			}
		}

		//preview of la_buff
		/*//if(petla<327)
		{
			cout<<endl<<"Rozmiar: "<<la_buff.size()<<" liczby: ";	
				for (map<int, int>::iterator k=la_buff.begin(); k!=la_buff.end(); ++k)
					cout<<k->second<<" ";
			cout<<endl;
		}*/
	
		la_begin=la_buff.begin();	
		s_begin=s_buff.begin();

		//searchnig the longest sequence
		max=1;
		for (map<int, int>::iterator k=s_end; k!=s_begin;)
		{
			--k;
			count=k;
			temp=la_begin;
			while (1)
			{
				if (count!=s_end)
					if (count->second==temp->second)
					{
						if (length>=9)
							{
								if_max_length_stop=1;
								break;											
							}
						++count;
						++temp;
						++length;
					}
					else	
						break;
				else
					break;
			}
			if (max<length)
			{
				max=length;
				position=k->first;
			}
			length=0;

			if (if_max_length_stop==1)
			{
				if_max_length_stop=0;
				break;
			}
		}

	//if (petla<327)
	//{
	//	cout<<endl<<"code: ";		
		//creating code of subpixels
		if (max==1)
		{
			//cout<<"0"<<la_begin->second;
			code=la_begin->second;
			ofile<<code;
		}
		else
		{
			code=128 | (max-2)<<4 | (position-s_begin->first+1);		
			//cout<<"1"<<max-2<<" "<<position-s_begin->first+1;
			ofile<<code;
		}
	//	if (petla==326)
	//		ofile.close();		
	//	cout<<endl;
	//}

	//	if (petla<327)
	//	cout<<"max dlugosc: "<<max<<endl;
		cpmax=max;

		//inserting elements into lookahead buffer
		while(max-->0)
		{
			s_buff.insert(pair<int, int>(la_buff_size+it,la_begin->second));		
			++la_begin;		
			++it;
		}
			
		//erase elements in search buffer
		max=cpmax;
		while(max-->0)
		{
			s_buff.erase(s_begin->first);
			++s_begin;
		}
		max=cpmax;

		//cout<<"petla: "<<petla;	

		//preview of s_buff
	  /*//if (petla<327)	
		{
			cout<<endl;
			for (map<int, int>::iterator k=s_begin; k!=s_end; ++k)
				cout<<k->second<<" ";	
		}*/
		
		//erase elements in la_buffer
		la_begin=la_buff.begin();
		while (max-->0)
		{
			la_buff.erase(la_begin->first);
			++la_begin;
		}

	}
	s_buff.clear();
	ofile.close();
}



void LZ77::decode()
{
	std::ifstream ifile("LZ77", std::ios::binary);
	unsigned int i;
	//unsigned int x=1, y=0; 				//coordinates of pixels
	Uint8 color[3], code, first_bit;
	//short what_color=1;					//which subpixel
	ifile>>code;
	color[0]=code<<4;
	cout<<(int)color[0]<<" F";


	//initialization of search buffer
	for (i=0; i<s_buff_size; ++i){
		s_buff.insert(pair<int, int>(i,(int)code));	
	cout<<s_buff[i]<<" ";}

	while (!ifile.eof())		
	{
		ifile>>code;
		cout<<(int)code<<" ";
		first_bit=code>>7;
		cout<<(int)first_bit<<" ";
		//if (first_bit)
		
	}	
	//cout<<"pixel"<<pixel<<"tu"<<endl;
	ifile.close();
}
