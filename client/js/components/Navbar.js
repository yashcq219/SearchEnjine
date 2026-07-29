import { checkHealth } from '../api.js';

export function initNavbar(store) {
    const healthBadge = document.getElementById('health-badge');
    const healthText = document.getElementById('health-text');
    const themeToggleBtn = document.getElementById('theme-toggle-btn');
    const themeIcon = document.getElementById('theme-icon');
    const brandElement = document.getElementById('nav-brand');

    // Apply initial theme
    const initialTheme = store.getState().theme;
    applyTheme(initialTheme);

    // Health check loop
    async function updateHealth() {
        const health = await checkHealth();
        store.setState({ isOnline: health.online });

        if (health.online) {
            healthBadge.className = 'health-badge online';
            healthText.textContent = 'Server Online';
        } else {
            healthBadge.className = 'health-badge offline';
            healthText.textContent = 'Backend Offline';
        }
    }

    updateHealth();
    setInterval(updateHealth, 10000);

    // Theme toggle listener
    themeToggleBtn.addEventListener('click', () => {
        const currentTheme = document.documentElement.getAttribute('data-theme') || 'dark';
        const newTheme = currentTheme === 'dark' ? 'light' : 'dark';
        applyTheme(newTheme);
        store.setState({ theme: newTheme });
        localStorage.setItem('searchengine-theme', newTheme);
    });

    function applyTheme(theme) {
        document.documentElement.setAttribute('data-theme', theme);
        if (theme === 'light') {
            themeIcon.className = 'fa-solid fa-moon';
        } else {
            themeIcon.className = 'fa-solid fa-sun';
        }
    }

    // Brand click resets to hero home view
    brandElement.addEventListener('click', () => {
        store.setState({
            query: '',
            results: [],
            hasSearched: false,
            error: null,
            isLoading: false
        });
        const searchInput = document.getElementById('search-input');
        if (searchInput) searchInput.value = '';
    });
}
