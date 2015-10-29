#pragma once

#include <vector>
#include <string>
#include <memory>
#include <mysql.h>


namespace mysql_helper
{
	class MySQLResult;
	typedef std::unique_ptr<MySQLResult> ResultPointer;

	/**
	 * MySQL客户端
	 */
	class MySQLClient
	{
	public:
		MySQLClient();
		~MySQLClient();

	public:
		/**
		 * 是否已连接
		 */
		bool is_connected() const
		{
			return connected_;
		}

		/**
		 * 已连接的数据库名
		 */
		const std::string& db_name() const
		{
			return db_name_;
		}

		/**
		 * 连接到数据库
		 * param host 主机地址
		 * param user 用户名
		 * param password 密码
		 * param db 数据库名
		 * param port 端口
		 */
		bool connect(const char *host, const char *user, const char *password, const char *db, unsigned int port);

		/**
		 * 执行查询操作
		 * @param sql sql语句
		 * @param result 查询结果
		 * @param affected_rows 影响行数
		 */
		bool query(const char *sql, ResultPointer *result = nullptr, my_ulonglong *affected_rows = nullptr);

		/**
		 * 断开连接
		 */
		void close();

	protected:
		MySQLClient(const MySQLClient &) = delete;
		MySQLClient& operator= (const MySQLClient &) = delete;

	private:
		bool			connected_;
		MYSQL			connect_;
		std::string		db_name_;
	};

	/**
	 * MySQL查询返回结果包装类
	 * @description 保证总能回收MYSQL_RES
	 */
	class MySQLResult
	{
	public:
		MySQLResult(MYSQL_RES *db_result);
		~MySQLResult();

	public:
		/**
		 * 行数
		 */
		my_ulonglong num_rows() const;

		/**
		 * 列数
		 */
		unsigned int num_fields() const;

		/**
		 * 取出所有列名
		 */
		std::vector<const char *> fetch_field() const;

		/**
		 * 取出某行数据
		 * @param row 行索引
		 */
		std::vector<const char *> fetch_row(my_ulonglong row = 0) const;

	protected:
		MySQLResult(const MySQLResult &) = delete;
		MySQLResult& operator= (const MySQLResult &) = delete;

	private:
		MYSQL_RES*	result_;
	};
}