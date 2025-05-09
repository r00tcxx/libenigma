#pragma once
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "bit_mask.h"
#include "class.h"
#include "types.h"

namespace enigma::db {
	enum class sqlite3_open_flags : int {
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

	BIT_MASK(sqlite3_open_flags);

	struct sqlite3_blob {
		void* data{nullptr};
		size_t size{0};

		sqlite3_blob() = default;
		sqlite3_blob(void* ptr, size_t sz) : data(ptr), size(sz) {}
	};

	using sqlite3_null	  = std::nullptr_t;
	using sqlite3_value	  = std::variant<i64, double, std::string, sqlite3_blob, sqlite3_null>;
	using sqlite3_columns = std::unordered_map<string, sqlite3_value>;
	using sqlite3_rows	  = std::vector<sqlite3_columns>;

	using sqlite3_result = std::pair<bool, string>;

	struct sqlite3_context {
		void* ctx;
		bool done{false};
	};

	class sqlite3cpp : no_copyable {
	   public:
		sqlite3cpp();
		~sqlite3cpp();

		sqlite3_result open(const string& db, const sqlite3_open_flags flags);
		void close();
		string error();
		sqlite3_result begin();
		sqlite3_result commit();
		sqlite3_result rollback();
		sqlite3_result exec(const string& sql, sqlite3_rows& rows);
		sqlite3_result exec(const string& sql);
		sqlite3_result prepare(const string& sql, sqlite3_context& ctx);
		sqlite3_result bind_value(sqlite3_context& ctx, sqlite3_value&& value);
		sqlite3_result bind_values(sqlite3_context& ctx, std::vector<sqlite3_value>&& values);
		sqlite3_result step(sqlite3_context& ctx);
		sqlite3_result step(sqlite3_context& ctx, sqlite3_columns& cols);
		sqlite3_result finalize(sqlite3_context& ctx);

		template <typename T>
		sqlite3_value make_value(T&& val) {
			using DT = std::decay_t<T>;

			if constexpr (std::is_convertible_v<DT, i64>) return static_cast<i64>(std::forward<T>(val));
			else if constexpr (std::is_convertible_v<DT, double>) return static_cast<double>(std::forward<T>(val));
			else if constexpr (std::is_convertible_v<DT, std::string>)
				return static_cast<std::string>(std::forward<T>(val));
			else if constexpr (std::is_convertible_v<DT, sqlite3_blob>) return std::forward<T>(val);
			else return nullptr;
		}

	   private:
		class impl;
		unique_ptr<impl> impl_;
	};
}  // namespace enigma::db
