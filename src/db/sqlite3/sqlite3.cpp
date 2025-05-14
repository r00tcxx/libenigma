#include "db/sqlite3/sqlite3.h"
#include "core/sqlite3.h"
#include "sqlite3_impl.h"

namespace ema::db {
	sqlite3cpp::sqlite3cpp() : impl_(make_unique<impl>()) {
		//
	}

	sqlite3cpp::~sqlite3cpp() {
		sqlite3cpp::close();
	}

	sqlite3_result sqlite3cpp::open(const string& db, const sqlite3_open_flags flags) {
		return impl_->open(db, flags);
	}

	void sqlite3cpp::close() {
		impl_->close();
	}

	string sqlite3cpp::error() {
		return impl_->error();
	}

	sqlite3_result sqlite3cpp::begin() {
		return impl_->begin();
	}

	sqlite3_result sqlite3cpp::commit() {
		return impl_->commit();
	}

	sqlite3_result sqlite3cpp::rollback() {
		return impl_->rollback();
	}

	sqlite3_result sqlite3cpp::exec(const string& sql, sqlite3_rows& rows) {
		return impl_->exec(sql, rows);
	}

	sqlite3_result sqlite3cpp::exec(const string& sql) {
		return impl_->exec(sql);
	}
	sqlite3_result sqlite3cpp::prepare(const string& sql, sqlite3_context& ctx) {
		return impl_->prepare(sql, ctx);
	}
		
	sqlite3_result sqlite3cpp::bind_value(sqlite3_context& ctx, sqlite3_value&& value) {
		return impl_->bind_value(ctx, std::move(value));
	}

	sqlite3_result sqlite3cpp::bind_values(sqlite3_context& ctx, std::vector<sqlite3_value>&& values) {
		return impl_->bind_values(ctx, std::move(values));
	}

	sqlite3_result sqlite3cpp::step(sqlite3_context& ctx, sqlite3_columns& cols) {
		return impl_->step(ctx, cols);
	}

	sqlite3_result sqlite3cpp::step(sqlite3_context& ctx) {
		return impl_->step(ctx);
	}

	sqlite3_result sqlite3cpp::finalize(sqlite3_context& ctx) {
		return impl_->finalize(ctx);
	}
}  // namespace enigma::db
