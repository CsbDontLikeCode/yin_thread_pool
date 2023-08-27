# yin_thread_pool
Thread Pool Learning<br>
yin_thread_pool使用案例:<br>
`ThreadPool pool(3); // 新建线程池并制定线程数`<br>
`pool.enqueue(func_name, args);`<br>

## 01.C++11 线程库基本使用
### 创建线程 
写法:std::thread t(func_name, args...);<br>
注意:线程在与主线程之间是各自独立的，有可能发生主线程运行结束而当前线程仍需运行的情况，这时候程序就会报错。<br>
t.join() 可让主程序等待当前线程结束，其本质是阻塞主线程。<br>
t.detach() 让当前线程与主线程分离。detach()往往应用在多进程之中。<br>
t.joinable() 返回bool值判定当前线程是否可以调用join()或detach()。<br>

## 02.线程中数据未定义错误
传递临时变量问题: 要通过std::ref()对线程参数作引用传递。<br>
传递指针或引用指向临时变量的问题:确保传入现成的参数并非临时变量。<br>
类成员函数作为入口函数，类对象被提前释放：使用智能指针。<br>

## 03.互斥量解决多线程数据共享问题
声明std::mutex，在需要对数据加锁是lock(),操作完数据后unlock()。<br>

## 04.互斥量死锁
按顺序加锁。<br>

## 05.lock_guard和unique_lock
std::lock_guard调用时自动加锁，离开作用域后自动解锁。<br>
std::lock_guadr本质是个模板类，构造时对传入的mutex锁进行lock(),析构时对传入的mutex锁进行unlock()。<br>
unique_lock也是调用时自动加锁，离开作用域时自动解锁,但是更加灵活，有lock(), try_lock_for()，try_lock_until<br>

## 06.call_once
保证无论多少个线程调用，某个函数只被调用一次。如在单例模式中，类只要被初始化一次。<br>
用法：std:once_flag flag;std::call_once(flag, func);<br>

## 07.condition_variable与其使用场景
声明条件变量:std::condition_varialbe cv;<br>
条件变量通知:cv.notify_once()或cv.notify_all().<br>
条件变量等待:cv.wait(lock, wait_flag);lock必须是unique锁.<br>