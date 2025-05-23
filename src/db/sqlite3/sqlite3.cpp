#include "db/sqlite3/sqlite3.h"
#include "core/sqlite3.h"
#include "sqlite3_impl.h"

namespace ema::db {
	Sqlite3cpp::Sqlite3cpp() : impl_(std::make_unique<Impl>()) {
	}

	Sqlite3cpp::~Sqlite3cpp() {
	}

	Sqlite3Result Sqlite3cpp::Open(const std::string& db, const Sqlite3OpenFlags flags) {
		return impl_->Open(db, flags);
	}

	void Sqlite3cpp::Close() {
		impl_->Close();
	}

	std::string Sqlite3cpp::Error() {
		return impl_->Error();
	}

	Sqlite3Result Sqlite3cpp::Begin() {
		return impl_->Begin();
	}

	Sqlite3Result Sqlite3cpp::Commit() {
		return impl_->Commit();
	}

	Sqlite3Result Sqlite3cpp::Rollback() {
		return impl_->Rollback();
	}

	Sqlite3Result Sqlite3cpp::Exec(const std::string& sql, Sqlite3Rows& rows) {
		return impl_->Exec(sql, rows);
	}

	Sqlite3Result Sqlite3cpp::Exec(const std::string& sql) {
		return impl_->Exec(sql);
	}
	Sqlite3Result Sqlite3cpp::Prepare(const std::string& sql, Sqlite3Context& ctx) {
		return impl_->Prepare(sql, ctx);
	}

	Sqlite3Result Sqlite3cpp::BindValue(Sqlite3Context& ctx, Sqlite3Value&& value) {
		return impl_->BindValue(ctx, std::move(value));
	}

	Sqlite3Result Sqlite3cpp::BindValues(Sqlite3Context& ctx, std::vector<Sqlite3Value>&& values) {
		return impl_->BindValues(ctx, std::move(values));
	}

	Sqlite3Result Sqlite3cpp::Step(Sqlite3Context& ctx, Sqlite3Columns& cols) {
		return impl_->Step(ctx, cols);
	}

	Sqlite3Result Sqlite3cpp::Step(Sqlite3Context& ctx) {
		return impl_->Step(ctx);
	}

	Sqlite3Result Sqlite3cpp::Finalize(Sqlite3Context& ctx) {
		return impl_->Finalize(ctx);
	}
}  // namespace ema::db
