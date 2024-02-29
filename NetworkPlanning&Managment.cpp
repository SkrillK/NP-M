#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

int N;
vector<int> critWay;
vector<int> workTime;
int critWayLR, critWayRL;

vector<vector<int>> matrixRead()
{
	ifstream file;

	vector<vector<int>> matrix;

	file.open("matrix.txt");
	if (file.is_open())
	{
		vector<int> row;
		string s1, s2;

		while (getline(file, s1, '\n'))
		{
			if (s1 == "")
				continue;

			stringstream line(s1);

			while (getline(line, s2, ' '))
				row.push_back(stoi(s2));

			matrix.push_back(row);
			row.clear();
		}
	}

	return matrix;
}

void topologicalSort(int m, vector<int>& visitedVrtx, vector<int>& sortResult, vector<vector<int>>& matrix)
{
	visitedVrtx[m] = 1;
	for (int i = 0; i < N; i++)
		if (matrix[m][i] == 1 && visitedVrtx[i] == 0)
			topologicalSort(i, visitedVrtx, sortResult, matrix);

	sortResult.push_back(m);
}

int sourceFind(vector<vector<int>>& matrix)
{
	int i, j, k = 0;
	bool flag;
	vector<int> source(N);

	for (i = 0; i < N; i++)
	{
		flag = true;
		for (j = 0; j < N; j++)
			if (matrix[j][i] == 1) flag = false;
		if (flag) { source[i] = 1; k++; }
	}
	i = 0;

	if (k == 1)
	{
		while (source[i] == 0) i++;
		return i;
	}
	else if (k == 0) return -1;
	else
	{
		vector<vector<int>> newMatrix(N + 1, vector<int>(N + 1));

		for (i = 0; i < N; i++)
		{
			matrix[0][i + 1] = source[i];
			for (j = 0; j < N; j++)
				newMatrix[i + 1][j + 1] = matrix[i][j];
		}

		matrix = newMatrix;
		N++;

		return 0;
	}
}

bool cycleFind(vector<vector<int>>& matrix)
{
	vector<vector<int>> M1 = matrix, M2(N, vector<int>(N));

	for (int l = 1; l < N; l++)
	{
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				M2[i][j] = 0;
				for (int k = 0; k < N; k++)
					M2[i][j] = (int)(M2[i][j] || M1[i][k] && matrix[k][i]);
			}
			if (M2[i][i] != 0) return false;
		}
		M1 = M2;
	}

	return true;
}

void transpos(vector<int>& workLength, vector<vector<int>>& matrix, vector<int>& sortResult)
{
	vector<int> newWorkLength(N);
	vector<vector<int>> newMatrix(N, vector<int>(N));

	for (int i = 0; i < N; i++)
	{
		int l = 0;

		while (sortResult[l] != i) 
			l++;

		newWorkLength[l] = workLength[i];
		for (int j = 0; j < N; j++)
			if (matrix[i][j] == 1)
			{
				int k = 0;
				while (sortResult[k] != j) k++;
				newMatrix[l][k] = 1;
			}
	}

	matrix = newMatrix;
	workLength = newWorkLength;
}

vector<int> workLR(vector<int>& workLength, vector<vector<int>>& matrix)
{
	vector<int> T(N);

	for (int i = 0; i < N; i++)
		for (int j = i + 1; j < N; j++)
			if (matrix[i][j] == 1 && T[j] < T[i] + workLength[i])
				T[j] = T[i] + workLength[i];
	critWayLR = T[N - 1] + workLength[N - 1];

	return T;
}

vector<int> workRL(vector<int>& workLength, vector<vector<int>>& matrix)
{
	vector<int> T1(N);

	for (int i = N - 1; i >= 0; i--)
		for (int j = i - 1; j >= 0; j--)
			if (matrix[j][i] == 1 && T1[j] < T1[i] + workLength[i])
				T1[j] = T1[i] + workLength[i];
	critWayRL = T1[0] + workLength[0];

	return T1;
}

void critWayFind(vector<int>& workLength, vector<vector<int>>& matrix, vector<int>& T, vector<int>& T1)
{
	cout << "Vertexes in crit. way: ";
	for (int i = 0; i < N; i++)
	{
		int buff = workLength[i] + T[i] + T1[i];
		critWay.push_back(buff);
		if (buff == critWayLR)
			cout << i << " ";
		workTime.push_back(critWayLR - buff);
	}
	cout << "\n";
}

int main()
{
	vector<vector<int>> inputMatrix;
	vector<int> workLength, sortResult;

	inputMatrix = matrixRead();
	N = inputMatrix[0].size();
	workLength = inputMatrix[N];
	inputMatrix.erase(inputMatrix.begin() + N);

	//--------------------------------------------------

	cout << "Input matrix:\n";
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++)
			cout << inputMatrix[i][j] << " ";
		cout << "\n";
	}

	cout << "\nLength of work:\n";
	for (int i = 0; i < N; i++)
		cout << workLength[i] << " ";
	cout << "\n\n";

	//--------------------------------------------------

	int source;

	if (!cycleFind(inputMatrix))
	{
		cout << "Cycle found.\n";
		exit(-1);
	}
	else
		cout << "Cycles not found.\n\n";

	source = sourceFind(inputMatrix);
	if (source < 0)
	{
		cout << "No source.";
		exit(-2);
	}
	else cout << "Source: " << source << "\n\n";

	//--------------------------------------------------

	vector<int> visitedVrtx(N);

	topologicalSort(source, visitedVrtx, sortResult, inputMatrix);
	reverse(sortResult.begin(), sortResult.end());

	cout << "Old numbers: ";
	for (int i = 0; i < N; i++)
		cout << sortResult[i] << " ";

	cout << "\nNew numbers: ";
	for (int i = 0; i < N; i++)
		cout << i << " ";
	cout << "\n";

	//--------------------------------------------------

	transpos(workLength, inputMatrix, sortResult);

	cout << "\nWork matrix (after sorting):\n";
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
			cout << inputMatrix[i][j] << " ";
		cout << "\n";
	}

	cout << "\nWork length matrix (after sorting):\n";
	for (int i = 0; i < N; i++) cout << workLength[i] << " ";
	cout << "\n\n";

	vector<int> T = workLR(workLength, inputMatrix);
	cout << "Critical way length(l to r): " << critWayLR << "\n";
	vector<int> T1 = workRL(workLength, inputMatrix);
	cout << "Critical way length(r to l): " << critWayRL << "\n\n";

	critWayFind(workLength, inputMatrix, T, T1);
	cout << "Time stock: ";
	for (int i = 0; i < N; i++)
		cout << workTime[i] << " ";
	cout << "\n";
}
