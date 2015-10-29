#include "mysql_helper.h"

#include <cassert>
#include <iostream>


namespace mysql_helper
{
	MySQLClient::MySQLClient()
		: connected_(false)
	{

	}

	MySQLClient::~MySQLClient()
	{
		close();
	}

	// 连接到数据库
	bool MySQLClient::connect(const char *host, const char *user, const char *password, const char *db, unsigned int port)
	{
		if (is_connected())
		{
			close();
		}

		mysql_init(&connect_);
		if (mysql_real_connect(&connect_, host, user, password, db, port, nullptr, 0) != nullptr)
		{
			db_name_ = db;
			connected_ = true;
		}
		else
		{
			mysql_close(&connect_);
			std::cerr << "Failed to connect to database: " << mysql_error(&connect_) << std::endl;
		}
		return connected_;
	}

	// 执行查询操作
	bool MySQLClient::query(const char *sql, ResultPointer *result, my_ulonglong *affected_rows)
	{
		if (!is_connected() || sql == nullptr)
		{
			return false;
		}

		if (mysql_query(&connect_, sql) == 0)
		{
			MYSQL_RES *db_result = mysql_store_result(&connect_);
			if (db_result != nullptr && result != nullptr)
			{
				*result = std::make_unique<MySQLResult>(db_result);
			}

			if (affected_rows != nullptr)
			{
				*affected_rows = mysql_affected_rows(&connect_);
			}
			return true;
		}
		else
		{
			std::cerr << "Failed to query database: " << mysql_error(&connect_) << std::endl;
			return false;
		}
	}

	// 断开连接
	void MySQLClient::close()
	{
		if (is_connected())
		{
			mysql_close(&connect_);
			db_name_ = "";
			connected_ = false;
		}
	}

	/************************************************************************/

	MySQLResult::MySQLResult(MYSQL_RES *db_result)
		: result_(db_result)
	{
		assert(result_ != nullptr);
	}

	MySQLResult::~MySQLResult()
	{
		if (result_ != nullptr)
		{
			mysql_free_result(result_);
		}
	}

	// 行数
	my_ulonglong MySQLResult::num_rows() const
	{
		assert(result_ != nullptr);
		return result_ == nullptr ? 0 : mysql_num_rows(result_);
	}

	// 列数
	unsigned int MySQLResult::num_fields() const
	{
		assert(result_ != nullptr);
		return result_ == nullptr ? 0 : mysql_num_fields(result_);
	}

	// 取出所有列名
	std::vector<const char *> MySQLResult::fetch_field() const
	{
		assert(result_ != nullptr);
		std::vector<const char *> ret;
		if (result_ != nullptr)
		{
			MYSQL_FIELD *fd = nullptr;
			while ((fd = mysql_fetch_field(result_)) != nullptr)
			{
				ret.push_back(fd->name);
			}
		}
		return ret;
	}

	// 取出某行数据
	std::vector<const char *> MySQLResult::fetch_row(my_ulonglong row) const
	{
		assert(result_ != nullptr);
		std::vector<const char *> ret;
		if (result_ != nullptr)
		{
			ret.reserve(num_rows());
			my_ulonglong index = 0;
			MYSQL_ROW row_data = nullptr;
			for (index; (row_data = mysql_fetch_row(result_)) != nullptr; ++index)
			{
				if (index == row)
				{
					for (unsigned int i = 0; i < num_fields(); ++i)
					{
						ret.push_back(row_data[i]);
					}
					break;
				}
			}
		}
		return ret;
	}
}