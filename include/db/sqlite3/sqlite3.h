#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "../../bit_mask.h"
#include "../../class.h"

namespace ema::db {
	enum class Sqlite3OpenFlags : int {
		READ		   = 0x00000001,
		READ_WRITE	   = 0x00000002,
		CREATE		   = 0x00000004,
		DELTE_ON_CLOSE = 0x00000008,
		EXCLUSIVE	   = 0x00000010,
		AUTO_PROXY	   = 0x00000020,
		URI			   = 0x00000040,
		MEMORY		   = 0x00000080,
		MAIN_DB		   = 0x00000100,
		TEMP_DB		   = 0x00000200,
		TRANSIENT_DB   = 0x00000400,
		MAIN_JOURNAL   = 0x00000800,
		TEMP_JOURNAL   = 0x00001000,
		SUB_JOURNAL	   = 0x00002000,
		SUPER_JOURNAL  = 0x00004000,
		NO_MUTEX	   = 0x00008000,
		FULL_MUTEX	   = 0x00010000,
		SHARED_CACHE   = 0x00020000,
		PRIVATE_CACHE  = 0x00040000,
		WAL			   = 0x00080000,
		NO_FOLLOW	   = 0x00100000,
		EXRESCODE	   = 0x00200000,
	};

	BIT_MASK(Sqlite3OpenFlags);

	struct Sqlite3Blob {
		void* data{nullptr};
		size_t size{0};

		Sqlite3Blob() = default;
		Sqlite3Blob(void* ptr, size_t sz) : data(ptr), size(sz) {
		}
	};

	using Sqlite3Null	 = std::nullptr_t;
	using Sqlite3Value	 = std::variant<long long, double, std::string, Sqlite3Blob, Sqlite3Null>;
	using Sqlite3Columns = std::unordered_map<std::string, Sqlite3Value>;
	using Sqlite3Rows	 = std::vector<Sqlite3Columns>;

	using Sqlite3Result = std::pair<bool, std::string>;

	struct Sqlite3Context {
		void* ctx;
		bool done{false};
	};

	class Sqlite3cpp : public NoCopyableMoveable {
	   public:
		Sqlite3cpp();
		~Sqlite3cpp();

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

		template <typename T>
		Sqlite3Value MakeValue(T&& val) {
			using DT = std::decay_t<T>;
			if constexpr (std::is_convertible_v<DT, long long>) return static_cast<long long>(std::forward<T>(val));
			else if constexpr (std::is_convertible_v<DT, double>) return static_cast<double>(std::forward<T>(val));
			else if constexpr (std::is_convertible_v<DT, std::string>)
				return static_cast<std::string>(std::forward<T>(val));
			else if constexpr (std::is_convertible_v<DT, Sqlite3Blob>) return std::forward<T>(val);
			else return nullptr;
		}

	   private:
		class Impl;
		std::unique_ptr<Impl> impl_;
	};
}  // namespace ema::db
