#include <vector>
#include <iostream>
#include "mysql_helper.h"

using namespace mysql_helper;


int main()
{
	MySQLClient client;
	ResultPointer retult;
	my_ulonglong affected_rows;
	if (client.connect("localhost", "root", "root", "mysql", 3306))
	{
		client.query("SELECT * FROM innodb_index_stats", &retult, &affected_rows);
		if (retult != nullptr)
		{
			int rows = retult->num_rows();
			for (int i = 0; i < rows; ++i)
			{
				auto data = retult->fetch_row(i);
				for (int j = 0; j < data.size(); ++j)
				{
					std::cout << (data[j] == NULL ? "NULL" : data[j]) << " ";
				}
				std::cout << std::endl;
			}
		}	
	}

	system("pause");
	return 0;
}