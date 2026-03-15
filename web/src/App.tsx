import { useEffect, useState } from "react";

function App() {
    const [count, setCount] = useState(0);

    useEffect(() => {
        const timer = setInterval(() => {
            setCount((prev) => prev + 1);
        }, 1000);

        return () => clearInterval(timer);
    }, []);

    return (
        <div className="bg-red-500 px-2 text-white">
            <h1>Hello World a</h1>
            <button onClick={() => setCount((prev) => prev + 1)}>
                Increment
            </button>
            <button onClick={() => setCount((prev) => prev - 1)}>
                Decrement
            </button>
            <button onClick={() => setCount(0)}>Reset</button>
            <p>Count: {count}</p>
        </div>
    );
}

export default App;
