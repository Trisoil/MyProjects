using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Reflection;
using System.Reflection.Emit;

namespace Synacast.ServicesFramework.Reflection
{
    public class ReflectionOptimizer
    {

        public static CreateInstanceInvoker CreateInstanceMethod(string type)
        {
            return CreateInstanceMethod(Type.GetType(type));
        }

        public static CreateInstanceInvoker CreateInstanceMethod(Type type)
        {
            if (type == null || type.IsInterface || type.IsAbstract)
            {
                throw new ApplicationException(string.Format("{0}不存在或者为抽象类型", type));
            }
            var method = new DynamicMethod(string.Empty, typeof(object), null, type, true);
            ILGenerator il = method.GetILGenerator();
            if (type.IsValueType)
            {
                LocalBuilder tmpLocal = il.DeclareLocal(type);
                il.Emit(OpCodes.Ldloca, tmpLocal);
                il.Emit(OpCodes.Initobj, type);
                il.Emit(OpCodes.Ldloc, tmpLocal);
                il.Emit(OpCodes.Box, type);
                il.Emit(OpCodes.Ret);

                return (CreateInstanceInvoker)method.CreateDelegate(typeof(CreateInstanceInvoker));
            }
            else
            {
                ConstructorInfo constructor = type.GetConstructor(BindingFlags.Instance | BindingFlags.Public |
                                                           BindingFlags.NonPublic, null, CallingConventions.HasThis, System.Type.EmptyTypes, null);
                if (constructor != null)
                {
                    il.Emit(OpCodes.Newobj, constructor);
                    il.Emit(OpCodes.Ret);

                    return (CreateInstanceInvoker)method.CreateDelegate(typeof(CreateInstanceInvoker));
                }
                else
                {
                    throw new ApplicationException(string.Format("{0}不存在默认的无参构造函数", type));
                }
            }
        }

        /// <summary>
        /// 生成赋值属性委托
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <typeparam name="V"></typeparam>
        /// <param name="property"></param>
        /// <returns></returns>
        public static Action<T,V> CreateSetPropertyDelegate<T,V>(PropertyInfo property)
        {
            var setMethod = property.GetSetMethod();
            return (Action<T, V>)Delegate.CreateDelegate(typeof(Action<T, V>), setMethod);
        }

        public static void SetProperty<T, V>(T obj, V value, string propertyName)
        {
            var property = obj.GetType().GetProperty(propertyName);
            var del = CreateSetPropertyDelegate<T, V>(property);
            del(obj, value);
        }

        public static V GetProperty<T, V>(T obj, string propertyName)
        {
            var property = obj.GetType().GetProperty(propertyName);
            var getMethod = property.GetGetMethod();
            var del = (Func<T,V>)Delegate.CreateDelegate(typeof(Func<T,V>), getMethod);
            return del(obj);
        }
    }
}
