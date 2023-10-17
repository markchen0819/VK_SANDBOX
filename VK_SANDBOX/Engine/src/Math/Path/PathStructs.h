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
	float denom = 0;

	// Run-time access table
	float GetUParameter(float arclength)
	{
		assert(!table.empty(), "Empty ArcLengthTable");

		assert(!(arclength <= 0.0f), "Invalid arclength input");
		assert(!(arclength >= table.back().arcLength), "Invalid arclength input");

		int index = BinarySearchArcLength(0, table.size() - 1, arclength);

		if(table[index].arcLength == arclength)
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
	float GetArcLength(float u)
	{
		assert(!table.empty(), "Empty ArcLengthTable");

		assert(!(u <= 0.0f), "Invalid u input");
		assert(!(u >= table.back().u), "Invalid u input");

		int index = BinarySearchUParameter(0, table.size() - 1, u);

		if (table[index].u == u)
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
	float LinearInterpolation(float x0, float x1, float y0, float y1, float x)
	{
		float t = (x - x0) / (x1 - x0);
		float result = (1 - t) * y0 + t * y1;
		return result;
	}
	int BinarySearchArcLength(int start, int end, float arcLength)
	{
		while (start <= end)
		{
			int mid = start + (end - start) / 2;

			// Check if it is at mid.
			if (table[mid].arcLength == arcLength)
			{
				return mid;
			}
			if (table[mid].arcLength < arcLength) // greater, ignore left
			{
				start = mid + 1;
			}
			else // smaller, ignore right
			{
				end = mid - 1;
			}
		}
		return start;
	}
	int BinarySearchUParameter(int start, int end, float u)
	{
		while (start <= end)
		{
			int mid = start + (end - start) / 2;

			// Check if it is at mid.
			if (table[mid].u == u)
			{
				return mid;
			}
			if (table[mid].u < u) // greater, ignore left
			{
				start = mid + 1;
			}
			else // smaller, ignore right
			{
				end = mid - 1;
			}
		}
		return start;
	}
	void Normalize()
	{
		assert(!table.empty() && "Empty ArcLengthTable");
		denom = table.back().arcLength; // last entry holds the total arc length

		normalizedTable.clear();
		for (const auto& entry : table)
		{
			ArcLengthEntry normalizedEntry;
			normalizedEntry.u = entry.u; // u remains the same.
			normalizedEntry.arcLength = entry.arcLength / denom; // Normalize the arc length
			normalizedTable.push_back(normalizedEntry);
		}
	}
	void Clear()
	{
		table.clear();
		normalizedTable.clear();
		denom = 0;
	}
	void PrintTable()
	{
		std::cout << "ArcLengthTable" << std::endl;
		for (const auto& entry : table)
		{
			std::cout << "U: " << entry.u << ", Arc Length: " << entry.arcLength << std::endl;
		}
		std::cout << "Normalized ArcLengthTable" << std::endl;
		for (const auto& entry : normalizedTable)
		{
			std::cout << "U: " << entry.u << ", Arc Length: " << entry.arcLength << std::endl;
		}
	}
	

};
