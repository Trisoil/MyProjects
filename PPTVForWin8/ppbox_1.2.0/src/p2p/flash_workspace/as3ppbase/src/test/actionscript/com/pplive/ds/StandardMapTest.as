package com.pplive.ds
{
	import com.pplive.ds.*;
	import com.pplive.test.object.valueEqualTo;
	
	import de.polygonal.ds.DA;
	import de.polygonal.ds.Itr;
	import de.polygonal.ds.Set;
	
	import com.pplive.mx.ObjectUtil;
	
	import org.flexunit.Assert;
	import org.hamcrest.assertThat;
	import org.hamcrest.collection.array;
	import org.hamcrest.collection.arrayWithSize;
	import org.hamcrest.collection.hasItem;
	import org.hamcrest.core.not;
	import org.hamcrest.object.equalTo;
	
	public class StandardMapTest
	{
		[Test]
		public function testInsertDel():void
		{
			var map:StandardMap = new StandardMap;
			
			Assert.assertTrue(map.isEmpty());
			
			// test insert
			Assert.assertTrue(map.set(1, 3));
			Assert.assertTrue(map.set(11, 13));
			Assert.assertTrue(map.set(2, 3));
			Assert.assertTrue(map.set(3, 4));
			Assert.assertFalse(map.set(2, 5));
			Assert.assertTrue(map.set(6, 5));
			
			// test existense
			trace(map);
			Assert.assertFalse(map.isEmpty());
			Assert.assertEquals(map.size(), 5);
			assertThat(map.begin(), valueEqualTo(new Pair(1, 3)));
			
			Assert.assertTrue(map.hasKey(1));
			Assert.assertTrue(map.hasKey(2));
			Assert.assertTrue(map.hasKey(3));
			Assert.assertFalse(map.hasKey(5));
			Assert.assertFalse(map.hasKey(0));
			
			Assert.assertTrue(map.has(3));
			Assert.assertTrue(map.has(4));
			Assert.assertTrue(map.has(5));
			Assert.assertTrue(map.contains(13));
			Assert.assertFalse(map.has(2));
			Assert.assertFalse(map.has(6));
			
			// test delete
			map.clr(1);
			Assert.assertFalse(map.hasKey(1));
			Assert.assertEquals(null, map.get(1));
			map.remove(2);
			Assert.assertFalse(map.hasKey(2));
			
			// test change value
			Assert.assertEquals(13, map.get(11));	
			Assert.assertTrue(map.remap(11, 15));
			Assert.assertEquals(15, map.get(11));
			
			// test delete all
			map.clear();
			Assert.assertEquals(0, map.size());
			Assert.assertFalse(map.hasKey(5));
			Assert.assertFalse(map.hasKey(0));
		}
		
		[Test]
		public function testComparableObjectInsertDel():void
		{
			var map:StandardMap = new StandardMap;
			
			Assert.assertTrue(map.isEmpty());
			
			// test insert
			Assert.assertTrue(map.set(new IntValueComparable(1), 1));
			Assert.assertTrue(map.set(new IntValueComparable(11), 13));
			Assert.assertTrue(map.set(new IntValueComparable(2), 3));
			Assert.assertTrue(map.set(new IntValueComparable(3), 4));
			Assert.assertFalse(map.set(new IntValueComparable(2), 5));
			Assert.assertTrue(map.set(new IntValueComparable(6), 7));
			
			// test existense
			trace(map);
			Assert.assertFalse(map.isEmpty());
			Assert.assertEquals(map.size(), 5);
			assertThat(map.begin(), valueEqualTo(new Pair(new IntValueComparable(1), 1)));
			
			Assert.assertTrue(map.hasKey(new IntValueComparable(1)));
			Assert.assertTrue(map.hasKey(new IntValueComparable(2)));
			Assert.assertTrue(map.hasKey(new IntValueComparable(3)));
			Assert.assertFalse(map.hasKey(new IntValueComparable(5)));
			Assert.assertFalse(map.hasKey(new IntValueComparable(0)));
			
			Assert.assertTrue(map.has(1));
			Assert.assertTrue(map.has(4));
			Assert.assertTrue(map.has(5));
			Assert.assertTrue(map.contains(13));
			Assert.assertTrue(map.contains(7));
			Assert.assertFalse(map.has(3));
			Assert.assertFalse(map.has(6));
			
			// test delete
			map.clr(new IntValueComparable(1));
			Assert.assertFalse(map.hasKey(new IntValueComparable(1)));
			Assert.assertEquals(null, map.get(new IntValueComparable(1)));
			map.clr(new IntValueComparable(2));
			Assert.assertFalse(map.hasKey(new IntValueComparable(2)));
			
			// test change value
			Assert.assertEquals(13, map.get(new IntValueComparable(11)));
			Assert.assertTrue(map.remap(new IntValueComparable(11), 15));
			Assert.assertEquals(15, map.get(new IntValueComparable(11)));
			
			// test delete all
			map.clear();
			Assert.assertEquals(0, map.size());
			Assert.assertFalse(map.hasKey(new IntValueComparable(5)));
			Assert.assertFalse(map.hasKey(new IntValueComparable(0)));
			
			// test key copy
			var obj:IntValueComparable = new IntValueComparable(1); 
			Assert.assertTrue(map.set(obj, 3));
			Assert.assertTrue(map.hasKey(obj));
			obj.val = 2;
			Assert.assertTrue(map.hasKey(obj));
			Assert.assertFalse(map.hasKey(new IntValueComparable(1)));
		}
		
		[Test]
		public function testNotComparableObjectInsertDel():void
		{
			var map:StandardMap = new StandardMap;
			
			Assert.assertTrue(map.isEmpty());
			
			// test insert
			Assert.assertTrue(map.set(new IntValue(1), 3));
			Assert.assertTrue(map.set(new IntValue(11), 13));
			Assert.assertTrue(map.set(new IntValue(2), 3));
			Assert.assertTrue(map.set(new IntValue(3), 4));
			Assert.assertFalse(map.set(new IntValue(2), 5));
			Assert.assertTrue(map.set(new IntValue(6), 5));
			
			// test existense
			trace(map);
			Assert.assertFalse(map.isEmpty());
			Assert.assertEquals(map.size(), 5);
			
			Assert.assertTrue(map.hasKey(new IntValue(1)));
			Assert.assertTrue(map.hasKey(new IntValue(2)));
			Assert.assertTrue(map.hasKey(new IntValue(3)));
			Assert.assertFalse(map.hasKey(new IntValue(5)));
			Assert.assertFalse(map.hasKey(new IntValue(0)));
			
			Assert.assertTrue(map.has(3));
			Assert.assertTrue(map.has(4));
			Assert.assertTrue(map.has(5));
			Assert.assertTrue(map.contains(13));
			Assert.assertFalse(map.has(2));
			Assert.assertFalse(map.has(6));
			
			// test delete
			map.clr(new IntValue(1));
			Assert.assertFalse(map.hasKey(new IntValue(1)));
			Assert.assertEquals(null, map.get(new IntValue(1)));
			map.clr(new IntValue(2));
			Assert.assertFalse(map.hasKey(new IntValue(2)));
			
			// test change value
			Assert.assertEquals(13, map.get(new IntValue(11)));
			Assert.assertTrue(map.remap(new IntValue(11), 15));
			Assert.assertEquals(15, map.get(new IntValue(11)));
			
			// test delete all
			map.clear();
			Assert.assertEquals(0, map.size());
			Assert.assertFalse(map.hasKey(new IntValue(5)));
			Assert.assertFalse(map.hasKey(new IntValue(0)));
		}
		
		[Test]
		public function testIterator():void
		{
			var map:StandardMap = initNumMap();
			
			var itemArray:Array = new Array;
			var iter:Itr = map.iterator();
			while(iter.hasNext())
			{
				itemArray.push(iter.next());
			}
			
			assertThat(itemArray, array(valueEqualTo(new Pair(1, 3)), valueEqualTo(new Pair(2, 3)), 
				valueEqualTo(new Pair(3, 4)), valueEqualTo(new Pair(6, 5)), valueEqualTo(new Pair(11, 13))));
			
			iter.reset();
			itemArray = new Array;
			while(iter.hasNext())
			{
				itemArray.push(iter.next());
			}
			
			assertThat(itemArray, array(valueEqualTo(new Pair(1, 3)), valueEqualTo(new Pair(2, 3)), 
				valueEqualTo(new Pair(3, 4)), valueEqualTo(new Pair(6, 5)), valueEqualTo(new Pair(11, 13))));
		}
		
		[Test]
		public function testObjectIterator():void
		{
			var map:StandardMap = initObjectMap();
			
			var itemArray:Array = new Array;
			var iter:Itr = map.iterator();
			while(iter.hasNext())
			{
				itemArray.push(iter.next());
			}
			
			assertThat(itemArray, array(valueEqualTo(new Pair(new IntValue(1), 3)), 
				valueEqualTo(new Pair(new IntValue(2), 3)), 
				valueEqualTo(new Pair(new IntValue(3), 4)), 
				valueEqualTo(new Pair(new IntValue(6), 5)), 
				valueEqualTo(new Pair(new IntValue(11), 13))));
		}
		
		[Test]
		public function testConversion():void
		{
			var map:StandardMap = initNumMap();
			// keys
			var keyArray:Array = new Array;
			var iter:Itr = map.keys();
			while(iter.hasNext())
			{
				keyArray.push(iter.next());	
			}
			assertThat(keyArray, array(1, 2, 3, 6, 11));
			
			// toKeySet
			var keySet:Set = map.toKeySet();
			Assert.assertTrue(keySet.has(1));
			Assert.assertTrue(keySet.has(2));
			Assert.assertTrue(keySet.has(3));
			Assert.assertTrue(keySet.has(6));
			Assert.assertTrue(keySet.has(11));
			Assert.assertFalse(keySet.has(5));
			Assert.assertFalse(keySet.has(12));
			
			// toValSet
			var valSet:Set = map.toValSet();
			Assert.assertTrue(valSet.has(3));
			Assert.assertTrue(valSet.has(4));
			Assert.assertTrue(valSet.has(5));
			Assert.assertTrue(valSet.has(13));
			Assert.assertFalse(valSet.has(2));
			Assert.assertFalse(valSet.has(12));
			
			// toArray
			var valArray:Array = map.toArray();
			assertThat(valArray, array(3, 3, 4, 5, 13));
			
			// toDA
			var da:DA = map.toDA();
			var valArray2:Array = da.toArray();
			assertThat(valArray2, array(3, 3, 4, 5, 13));
		}
		
		[Test]
		public function testClone():void
		{
			var map:StandardMap = new StandardMap;
			map.set(7, new IntValueClonable(1));
			map.set(1, new IntValueClonable(2));
			map.set(3, new IntValueClonable(3));
			
			// test assign clone
			var map2:StandardMap = map.clone(true) as StandardMap;
			var iter:Itr = map.iterator();
			var iter2:Itr = map2.iterator();
			while(iter.hasNext())
			{
				Assert.assertTrue(iter2.hasNext());
				var pair:Pair = iter.next() as Pair;
				var pair2:Pair = iter2.next() as Pair;
				
				assertThat(pair.key, equalTo(pair2.key));
				assertThat(pair.value, equalTo(pair2.value));
			}
			
			iter.reset();
			iter2.reset();
			while(iter2.hasNext())
			{
				Assert.assertTrue(iter.hasNext());
				pair2 = iter2.next() as Pair;
				pair = iter.next() as Pair;				
				
				assertThat(pair2.key, equalTo(pair.key));
				assertThat(pair2.value, equalTo(pair.value));
			}
			
			// test not assign clone
			var map3:StandardMap = map.clone(false) as StandardMap;
			iter.reset();
			var iter3:Itr = map3.iterator();
			while(iter.hasNext())
			{
				Assert.assertTrue(iter3.hasNext());
				var pair_:Pair = iter.next() as Pair;
				var pair3:Pair = iter3.next() as Pair;
				
				assertThat(pair_.key, equalTo(pair3.key));
				assertThat(pair_.value, not(equalTo(pair3.value)));
				assertThat(pair_.value, valueEqualTo(pair3.value));
			}
			
			iter.reset();
			iter3.reset();
			while(iter3.hasNext())
			{
				Assert.assertTrue(iter.hasNext());
				pair3 = iter3.next() as Pair;
				pair_ = iter.next() as Pair;			
				
				assertThat(pair3.key, equalTo(pair_.key));
				assertThat(pair3.value, not(equalTo(pair_.value)));
				assertThat(pair3.value, valueEqualTo(pair_.value));
			}
		}
		
		[Test]
		public function testFree():void
		{
			// TODO(herain):I don't know how to test free
			var map:StandardMap = initNumMap();
			map.free();
		}
		
		private function initNumMap():StandardMap
		{
			var map:StandardMap = new StandardMap;
			Assert.assertTrue(map.set(1, 3));
			Assert.assertTrue(map.set(11, 13));
			Assert.assertTrue(map.set(2, 3));
			Assert.assertTrue(map.set(3, 4));
			Assert.assertTrue(map.set(6, 5));
			return map;
		}
		
		private function initObjectMap():StandardMap
		{
			var map:StandardMap = new StandardMap;
			Assert.assertTrue(map.set(new IntValue(1), 3));
			Assert.assertTrue(map.set(new IntValue(11), 13));
			Assert.assertTrue(map.set(new IntValue(2), 3));
			Assert.assertTrue(map.set(new IntValue(3), 4));
			Assert.assertTrue(map.set(new IntValue(6), 5));
			return map;
		}
	}
}