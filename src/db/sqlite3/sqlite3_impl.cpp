#include "sqlite3_Impl.h"

namespace ema::db {
	Sqlite3Result Sqlite3cpp::Impl::Open(const std::string& db, const Sqlite3OpenFlags flags) {
		if (db.empty()) return {false, "invalid file"};
		auto code = sqlite3_open_v2(db.c_str(), (sqlite3**)&db_, static_cast<int>(flags), nullptr);
		return SQLITE_OK == code ? std::make_pair(true, std::string()) : std::make_pair(false, Error());
	}

	void Sqlite3cpp::Impl::Close() {
		if (!db_) return;
		sqlite3_close_v2((sqlite3*)db_);
		db_ = nullptr;
	}

	std::string Sqlite3cpp::Impl::Error() {
		if (!db_) return "";
		auto code = sqlite3_errcode(db_);
		if (SQLITE_OK == code) return "";
		std::string error = sqlite3_errstr(code);
		sqlite3_free(db_);
		return error;
	}

	Sqlite3Result Sqlite3cpp::Impl::Begin() {
		if (!db_) return {false, "invalid db"};
		auto code = sqlite3_exec((sqlite3*)db_, "BEGIN", nullptr, nullptr, nullptr);
		return SQLITE_OK == code ? std::make_pair(true, std::string()) : std::make_pair(false, Error());
	}

	Sqlite3Result Sqlite3cpp::Impl::Commit() {
		if (!db_) return {false, "invalid db"};
		auto code = sqlite3_exec((sqlite3*)db_, "COMMIT", nullptr, nullptr, nullptr);
		return SQLITE_OK == code ? std::make_pair(true, std::string()) : std::make_pair(false, Error());
	}

	Sqlite3Result Sqlite3cpp::Impl::Rollback() {
		if (!db_) return {false, "invalid db"};
		auto code = sqlite3_exec((sqlite3*)db_, "ROLLBACK", nullptr, nullptr, nullptr);
		return SQLITE_OK == code ? std::make_pair(true, std::string()) : std::make_pair(false, Error());
	}

	Sqlite3Result Sqlite3cpp::Impl::Exec(const std::string& sql, Sqlite3Rows& rows) {
		if (sql.empty()) return {false, "invalid sql"};
		else if (!db_) return {false, "invalid db"};

		sqlite3_stmt* stmt{nullptr};
		auto code = sqlite3_prepare_v2((sqlite3*)db_, sql.c_str(), static_cast<int>(sql.size()), &stmt, nullptr);
		if (code != SQLITE_OK) {
			return {false, Error()};
		}
		rows.clear();
		for (code = sqlite3_step(stmt);;) {
			if (SQLITE_LOCKED == code || SQLITE_BUSY == code) continue;
			else if (SQLITE_DONE == code) {
				sqlite3_finalize(stmt);
				break;
			}
			else if (SQLITE_ROW == code) {
				auto column_count = sqlite3_column_count(stmt);
				Sqlite3Columns columns;
				for (int i = 0; i < column_count; ++i) {
					auto column_name = sqlite3_column_name(stmt, i);
					auto column_type = sqlite3_column_type(stmt, i);
					Sqlite3Value value;
					switch (column_type) {
						case SQLITE_INTEGER:
							value = sqlite3_column_int64(stmt, i);
							break;
						case SQLITE_FLOAT:
							value = sqlite3_column_double(stmt, i);
							break;
						case SQLITE_TEXT:
							value = std::string((char*)(sqlite3_column_text(stmt, i)));
							break;
						case SQLITE_BLOB: {
							Sqlite3Blob blob;
							blob.size = sqlite3_column_bytes(stmt, i);
							auto data = sqlite3_column_blob(stmt, i);
							if (blob.size > 0 && (blob.data = malloc(blob.size)))
								memcpy_s(blob.data, blob.size, data, blob.size);
							value = blob;

						} break;
						case SQLITE_NULL:
							value = nullptr;
							break;
						default:
							break;
					}
					columns[column_name] = std::move(value);
				}
				rows.emplace_back(columns);
			}
		}
		return {true, ""};
	}

	Sqlite3Result Sqlite3cpp::Impl::Exec(const std::string& sql) {
		Sqlite3Rows rows;
		return Exec(sql, rows);
	}

	Sqlite3Result Sqlite3cpp::Impl::Prepare(const std::string& sql, Sqlite3Context& ctx) {
		if (sql.empty()) return {false, "invalid sql"};
		auto code = sqlite3_prepare_v2((sqlite3*)db_, sql.c_str(), static_cast<int>(sql.size()),
									   (sqlite3_stmt**)&ctx.ctx, nullptr);
		return SQLITE_OK == code ? std::make_pair(true, std::string()) : std::make_pair(false, Error());
	}

	Sqlite3Result Sqlite3cpp::Impl::BindValue(Sqlite3Context& ctx, Sqlite3Value&& value) {
		if (!ctx.ctx) return {false, "invalid context"};
		auto code = SQLITE_OK;
		if (std::holds_alternative<long long>(value))
			code = sqlite3_bind_int64((sqlite3_stmt*)ctx.ctx, 1, std::get<long long>(value));
		else if (std::holds_alternative<double>(value))
			code = sqlite3_bind_double((sqlite3_stmt*)ctx.ctx, 1, std::get<double>(value));
		else if (std::holds_alternative<std::string>(value)) {
			auto& str = std::get<std::string>(value);
			code =
				sqlite3_bind_text((sqlite3_stmt*)ctx.ctx, 1, str.c_str(), static_cast<int>(str.size()), SQLITE_STATIC);
		}
		else if (std::holds_alternative<Sqlite3Blob>(value)) {
			auto blob = std::get<Sqlite3Blob>(value);
			code = sqlite3_bind_blob((sqlite3_stmt*)ctx.ctx, 1, blob.data, static_cast<int>(blob.size), SQLITE_STATIC);
		}
		else if (std::holds_alternative<Sqlite3Null>(value)) {
			code = sqlite3_bind_null((sqlite3_stmt*)ctx.ctx, 1);
		}
		return SQLITE_OK == code ? std::make_pair(true, std::string()) : std::make_pair(false, Error());
	}

	Sqlite3Result Sqlite3cpp::Impl::BindValues(Sqlite3Context& ctx, std::vector<Sqlite3Value>&& values) {
		for (auto&& v : values) {
			auto result = BindValue(ctx, std::move(v));
			if (!result.first) return result;
		}
		return {true, ""};
	}

	Sqlite3Result Sqlite3cpp::Impl::Step(Sqlite3Context& ctx) {
		Sqlite3Columns cols;
		return Step(ctx, cols);
	}

	Sqlite3Result Sqlite3cpp::Impl::Step(Sqlite3Context& ctx, Sqlite3Columns& cols) {
		if (!ctx.ctx) return {false, "invalid context"};
		cols.clear();
		sqlite3_stmt* stmt = (sqlite3_stmt*)ctx.ctx;
		auto code		   = sqlite3_step(stmt);
		if (code != SQLITE_ROW) return {false, Error()};
		auto column_count = sqlite3_column_count(stmt);
		for (int i = 0; i < column_count; ++i) {
			auto column_name = sqlite3_column_name(stmt, i);
			auto column_type = sqlite3_column_type(stmt, i);
			Sqlite3Value value;
			switch (column_type) {
				case SQLITE_INTEGER:
					value = sqlite3_column_int64(stmt, i);
					break;
				case SQLITE_FLOAT:
					value = sqlite3_column_double(stmt, i);
					break;
				case SQLITE_TEXT:
					value = std::string((char*)(sqlite3_column_text(stmt, i)));
					break;
				case SQLITE_BLOB: {
					Sqlite3Blob blob;
					blob.size = sqlite3_column_bytes(stmt, i);
					auto data = sqlite3_column_blob(stmt, i);
					if (blob.size > 0 && (blob.data = malloc(blob.size)))
						memcpy_s(blob.data, blob.size, data, blob.size);
					value = blob;

				} break;
				case SQLITE_NULL:
					value = nullptr;
					break;
				default:
					break;
			}
			cols[column_name] = std::move(value);
		}
		return {true, ""};
	}

	Sqlite3Result Sqlite3cpp::Impl::Finalize(Sqlite3Context& ctx) {
		if (!ctx.ctx) return {false, "invalid context"};
		auto code = sqlite3_finalize((sqlite3_stmt*)ctx.ctx);
		ctx.ctx	  = nullptr;
		return SQLITE_OK == code ? std::make_pair(true, std::string()) : std::make_pair(false, Error());
	}

}  // namespace ema::db
