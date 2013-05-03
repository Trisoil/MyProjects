package
{
	import com.pplive.ds.StandardMapTest;
	import com.pplive.test.object.ValueEqualToTest;
	import com.pplive.util.BitStreamTest;
	import com.pplive.util.EventUtilTest;
	import com.pplive.util.GUIDTest;
	import com.pplive.util.StringConvertTest;
	import com.pplive.util.URITest;
	
	import de.polygonal.ds.ArrayUtilTest;
	import de.polygonal.ds.HashMapTest;
	
	[Suite]
	[RunWith("org.flexunit.runners.Suite")]
	public class AllAS3PPBaseTest
	{
		public var maptest:StandardMapTest;
		public var valueequaltest:ValueEqualToTest;
		public var arrayUtilTest:ArrayUtilTest;
		public var uriTest:URITest;
		public var hashMapTest:HashMapTest;
		public var guidTest:GUIDTest;
		public var stringConvertTest:StringConvertTest;
		public var eventUtilTest:EventUtilTest;
		public var bitStreamTest:BitStreamTest;
	}
}