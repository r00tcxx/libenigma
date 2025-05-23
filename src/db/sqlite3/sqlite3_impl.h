#pragma once
#include "core/sqlite3.h"
#include "db/sqlite3/sqlite3.h"

namespace ema::db {
	class Sqlite3cpp::Impl {
	   public:
		Impl() = default;
		~Impl() {
			Close();
		}
		Sqlite3Result Open(const std::string& db, const Sqlite3OpenFlags flags);
		void Close();
		std::string Error();
		Sqlite3Result Begin();
		Sqlite3Result Commit();
		Sqlite3Result Rollback();
		Sqlite3Result Exec(const std::string& sql, Sqlite3Rows& rows);
		Sqlite3Result Exec(const std::string& sql);
		Sqlite3Result Prepare(const std::string& sql, Sqlite3Context& ctx);
		Sqlite3Result BindValue(Sqlite3Context& ctx, Sqlite3Value&& value);
		Sqlite3Result BindValues(Sqlite3Context& ctx, std::vector<Sqlite3Value>&& values);
		Sqlite3Result Step(Sqlite3Context& ctx);
		Sqlite3Result Step(Sqlite3Context& ctx, Sqlite3Columns& cols);
		Sqlite3Result Finalize(Sqlite3Context& ctx);

	   private:
		sqlite3* db_{nullptr};
	};
}  // namespace ema::db
