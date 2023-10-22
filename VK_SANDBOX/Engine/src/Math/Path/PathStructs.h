#pragma once

struct Segment
{
	float uStart;
	float uEnd;
	float length; // Linear approximation of the segment length
};

struct ArcLengthEntry
{
	float u;         // parameter u of the curve
	float arcLength; // accumulated arclength
};


struct ArcLengthTable
{
	std::vector<ArcLengthEntry> table;
	std::vector<ArcLengthEntry> normalizedTable;
	float totalU = 0;
	float totalArcLength = 0;

	// Run-time access table
	float GetUParameterFromTable(float arclength)
	{
		assert(!table.empty(), "Empty ArcLengthTable");

		assert(!(arclength <= 0.0f), "Invalid arclength input");
		assert(!(arclength > table.back().arcLength), "Invalid arclength input");

		int index = BinarySearchArcLength(0, table.size() - 1, arclength, table);
		index -= 1;
		if(table[index].arcLength == arclength || (index == table.size() - 1))
		{
			return table[index].u;
		}

		float result = LinearInterpolation
		(
			table[index].arcLength,
			table[index+1].arcLength,
			table[index].u,
			table[index+1].u,
			arclength
		);
		return result;
	}
	float GetArcLengthFromTable(float u)
	{
		assert(!table.empty(), "Empty ArcLengthTable");

		assert(!(u <= 0.0f), "Invalid u input");
		assert(!(u > table.back().u), "Invalid u input");

		int index = BinarySearchUParameter(0, table.size() - 1, u, table);
		index -= 1;
		if (table[index].u == u || (index == table.size() - 1))
		{
			return table[index].arcLength;
		}

		float result = LinearInterpolation
		(
			table[index].u,
			table[index + 1].u,
			table[index].arcLength,
			table[index + 1].arcLength,
			u
		);
		return result;
	}
	float GetUParameterFromNormalizedTable(float arclength)
	{
		assert(!normalizedTable.empty(), "Empty ArcLengthTable");

		assert(!(arclength <= 0.0f), "Invalid arclength input");
		assert(!(arclength > normalizedTable.back().arcLength), "Invalid arclength input");

		int index = BinarySearchArcLength(0, normalizedTable.size() - 1, arclength, normalizedTable);
		index -= 1;
		if (normalizedTable[index].arcLength == arclength || (index == normalizedTable.size() - 1))
		{
			return normalizedTable[index].u;
		}

		float result = LinearInterpolation
		(
			normalizedTable[index].arcLength,
			normalizedTable[index + 1].arcLength,
			normalizedTable[index].u,
			normalizedTable[index + 1].u,
			arclength
		);
		return result;
	}
	float GetArcLengthFromNormalizedTable(float u)
	{
		assert(!normalizedTable.empty(), "Empty ArcLengthTable");

		assert(!(u <= 0.0f), "Invalid u input");
		assert(!(u > normalizedTable.back().u), "Invalid u input");

		int index = BinarySearchUParameter(0, normalizedTable.size() - 1, u, normalizedTable);
		index -= 1;
		if (normalizedTable[index].u == u || (index == normalizedTable.size() - 1))
		{
			return normalizedTable[index].arcLength;
		}

		float result = LinearInterpolation
		(
			normalizedTable[index].u,
			normalizedTable[index + 1].u,
			normalizedTable[index].arcLength,
			normalizedTable[index + 1].arcLength,
			u
		);
		return result;
	}

	float LinearInterpolation(float x0, float x1, float y0, float y1, float x)
	{
		float t = (x - x0) / (x1 - x0);
		float result = (1 - t) * y0 + t * y1;
		return result;
	}
	int BinarySearchArcLength(int start, int end, float arcLength, std::vector<ArcLengthEntry>& targetTable)
	{
		while (start <= end)
		{
			int mid = start + (end - start) / 2;

			// Check if it is at mid.
			if (targetTable[mid].arcLength == arcLength)
			{
				return mid;
			}
			if (targetTable[mid].arcLength <= arcLength) // greater, ignore left
			{
				start = mid + 1;
			}
			else // smaller, ignore right
			{
				end = mid - 1;
			}
		}
		return start; // return smallest that is greater than the desired
	}
	int BinarySearchUParameter(int start, int end, float u, std::vector<ArcLengthEntry>& targetTable)
	{
		while (start <= end)
		{
			int mid = start + (end - start) / 2;

			// Check if it is at mid.
			if (targetTable[mid].u == u)
			{
				return mid;
			}
			if (targetTable[mid].u < u) // greater, ignore left
			{
				start = mid + 1;
			}
			else // smaller, ignore right
			{
				end = mid - 1;
			}
		}
		return start; // return smallest that is greater than the desired
	}
	void Normalize()
	{
		assert(!table.empty() && "Empty ArcLengthTable");
		totalArcLength = table.back().arcLength; // last entry holds the total arc length
		totalU = table.back().u;
		normalizedTable.clear();
		for (const auto& entry : table)
		{
			ArcLengthEntry normalizedEntry;
			normalizedEntry.u = entry.u/ totalU; // u remains the same.
			normalizedEntry.arcLength = entry.arcLength / totalArcLength; // Normalize the arc length
			normalizedTable.push_back(normalizedEntry);
		}
	}
	void Clear()
	{
		table.clear();
		normalizedTable.clear();
		totalArcLength = 0;
	}
	void PrintTable()
	{
		std::cout << "========================" << std::endl;
		std::cout << "ArcLengthTable" << std::endl;
		for (const auto& entry : table)
		{
			std::cout << "U: " << entry.u << ", Arc Length: " << entry.arcLength << std::endl;
		}
		std::cout << "Normalized ArcLengthTable" << std::endl;
		std::cout << "totalU: " << totalU << std::endl;
		std::cout << "totalArcLength: " << totalArcLength << std::endl;
		for (const auto& entry : normalizedTable)
		{
			std::cout << "U: " << entry.u << ", Arc Length: " << entry.arcLength << std::endl;
		}
		std::cout << "========================" << std::endl;
	}

	// global ArcLengthTable indexing
	std::vector<float> subCurveRatio;
	std::vector<float> accumulatedSubCurveRatio;
	int GetSubCurveIndex(float normalizedU)
	{
		assert(normalizedU >= 0.0f && normalizedU <= 1.0f, "normalizedU must be between 0.0 and 1.0");

		// Binary search to find the first element not less than normalizedU
		auto lower = std::lower_bound(accumulatedSubCurveRatio.begin(), accumulatedSubCurveRatio.end(), normalizedU);
		if (lower != accumulatedSubCurveRatio.end())
		{
			// On the boundary, belongs to higher curve
			int subCurveIndex = std::distance(accumulatedSubCurveRatio.begin(), lower);
			// Not the first subCurve
			if (subCurveIndex > 0)
			{
				subCurveIndex -= 1;
			}
			return subCurveIndex;
		}
		return accumulatedSubCurveRatio.size() - 1;
	}
	float GetLocalUParameter(float normalizedU, int targetSubCurveIndex)
	{
		float accumulatedRatio = accumulatedSubCurveRatio[targetSubCurveIndex];
		//float accumulatedRatio = 0.0f;
		//for (int i = 0; i < targetSubCurveIndex; ++i)
		//{
		//	accumulatedRatio += subCurveRatio[i];
		//}
		float localU = (normalizedU - accumulatedRatio) / subCurveRatio[targetSubCurveIndex];
		return localU;
	}

};
