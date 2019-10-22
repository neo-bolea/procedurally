/*#pragma once

#include "StrongTypes.h"

namespace Units
{
	struct Seconds : StrongTypedef<Seconds, double>, mixedRealArithmetic<Seconds, double>,
		numberConvertible<Seconds>
	{ 
		using StrongTypedef::StrongTypedef; 
		using numberConvertible::numberConvertible;
	};
}

Units::Seconds operator ""   h(long double val) { return Units::Seconds(val * 60 * 60); }
Units::Seconds operator "" min(long double val) { return Units::Seconds(val * 60); }
Units::Seconds operator ""   s(long double val) { return Units::Seconds(val * 1); }
Units::Seconds operator ""  ms(long double val) { return Units::Seconds(val * 0.001); }
Units::Seconds operator ""  µs(long double val) { return Units::Seconds(val * 0.000001); }
Units::Seconds operator ""  ns(long double val) { return Units::Seconds(val * 0.000000001); }*/