#include <vector>

using namespace std;

struct coord
{
	size_t start; 
	size_t end;
	
	size_t GetSize() const
	{
		return end - start;
	}
};
////////////////////////////////////////////////////////////////////////////////

template<typename T>
void MergeSort(vector<T>& v, int K)
{
	// рассчет количества элементов в каждом куске
	int width = v.size() / K; 
	
	if(width * K < v.size())
		width++;
	
	// массив начала и концов
	coord mas[K];
	
	#pragma omp parallel for
	for(int i = 0; i < K; i++)
	{	
		mas[i].start = i * width;
		mas[i].end = (i == K - 1) ? v.size() :  (i + 1) * width;
		
		sort(v.begin() + mas[i].start, v.begin() + mas[i].end);
	}
	
	vector<T> temp(v.size());
	
	while( width < v.size() )
	{
		#pragma omp parallel for ordered  
		for(int i = 0; i < K / 2; i++)
		{		
			Merger(v, temp, mas[i * 2], mas[i * 2 + 1]);
			
			#pragma omp ordered
			{
				mas[i].start = mas[i * 2].start;
				mas[i].end = mas[i * 2 + 1].end;
			}
		}
		
		width *= 2;
		K /= 2;
		swap(v, temp);
	}
}

////////////////////////////////////////////////////////////////////////////////
// Функция слияния
////////////////////////////////////////////////////////////////////////////////
template<typename T>
void Merger(vector<T>& v, vector<T>& temp, const coord& mas1, const coord& mas2)
{
	// рассчет начальных индексов
	int x1 = mas1.start, x2 = mas2.start, x3 = mas1.start;
	int	y1 = mas1.end - 1, y2 = mas2.end - 1;
	
	// слияние пока не совпадут индексы, что означает опустошение одного из кусков
	while(x1 <= y1 && x2 <= y2)
	{
		// вставка из первого куска, движение индексов слева
		if(v[x1] <= v[x2])
		{
			swap(temp[x3], v[x1]);
			x3++;
			x1++;
		}
		// вставка из второго куска, движение индексов слева
		else
		{
			swap(temp[x3], v[x2]);
			x3++;
			x2++;
		}
	}
	
	// если первый кусок еще не опустошен, то сливаем оставшиеся элементы 
	for(int i = x1; i <= y1; i++)
	{
		swap(temp[x3], v[i]);
		x3++;
	}
	
	// если второй кусок еще не опустошен, то сливаем оставшиеся элементы	 
	for(int i = x2; i <= y2; i++)
	{
		swap(temp[x3], v[i]);
		x3++;
	}
}