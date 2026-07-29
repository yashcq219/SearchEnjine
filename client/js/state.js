/**
 * Application State Manager
 */
class StateManager {
    constructor() {
        this.state = {
            query: '',
            results: [],
            isLoading: false,
            hasSearched: false,
            error: null,
            durationMs: 0,
            isOnline: false,
            theme: localStorage.getItem('searchengine-theme') || 'dark'
        };
        this.listeners = [];
    }

    getState() {
        return this.state;
    }

    setState(newState) {
        this.state = { ...this.state, ...newState };
        this.notify();
    }

    subscribe(listener) {
        this.listeners.push(listener);
        return () => {
            this.listeners = this.listeners.filter(l => l !== listener);
        };
    }

    notify() {
        this.listeners.forEach(listener => listener(this.state));
    }
}

export const store = new StateManager();
