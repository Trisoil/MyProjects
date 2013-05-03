package com.pplive.util
{
	public class MathUtil
	{
		public function MathUtil()
		{
		}
		
		public static function limitMinMaxUint(value:uint, min:uint, max:uint):uint
		{
			return value < min ? min : (value > max ? max : value); 
		}
		
		public static function limitMaxUint(value:uint, max:uint):uint
		{
			return value < max ? value : max;
		}
	}
}