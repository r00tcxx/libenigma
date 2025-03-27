/*
 * Created on Mon Mar 24 2025
 *
 * The MIT License (MIT)
 * Copyright (c) 2025 enigma
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 * TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#pragma once
namespace enigma {
	class no_moveable {
	   protected:
		no_moveable() {}
		virtual ~no_moveable() = default;

		no_moveable(no_moveable&&)			  = delete;
		no_moveable& operator=(no_moveable&&) = delete;
	};

	class no_copyable {
	   protected:
		no_copyable()		   = default;
		virtual ~no_copyable() = default;

		no_copyable(const no_copyable&)		 = delete;
		no_copyable& operator=(no_copyable&) = delete;
	};

	class no_cmable : public no_copyable, public no_moveable {
	   protected:
		no_cmable()			 = default;
		virtual ~no_cmable() = default;
	};

	class no_constructiable {
	   private:
		no_constructiable() = delete;
	};

}  // namespace enigma
