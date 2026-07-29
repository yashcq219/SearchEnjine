import { search } from '../api.js';

export function initSearchBar(store) {
    const form = document.getElementById('search-form');
    const input = document.getElementById('search-input');
    const clearBtn = document.getElementById('search-clear-btn');
    const tagChipsContainer = document.getElementById('tag-chips');

    // Handle input typing & clear button visibility
    input.addEventListener('input', () => {
        if (input.value.trim().length > 0) {
            clearBtn.classList.add('visible');
        } else {
            clearBtn.classList.remove('visible');
        }
    });

    clearBtn.addEventListener('click', () => {
        input.value = '';
        clearBtn.classList.remove('visible');
        input.focus();
    });

    // Form submission
    form.addEventListener('submit', async (e) => {
        e.preventDefault();
        const query = input.value.trim();
        if (!query) return;

        executeSearch(query);
    });

    // Tag chip click handlers
    tagChipsContainer.addEventListener('click', (e) => {
        const chip = e.target.closest('.tag-chip');
        if (chip) {
            const query = chip.getAttribute('data-query');
            if (query) {
                input.value = query;
                clearBtn.classList.add('visible');
                executeSearch(query);
            }
        }
    });

    async function executeSearch(query) {
        store.setState({
            query,
            isLoading: true,
            hasSearched: true,
            error: null
        });

        const res = await search(query);

        if (res.success) {
            store.setState({
                results: res.results,
                isLoading: false,
                durationMs: res.durationMs,
                error: null
            });
        } else {
            store.setState({
                results: [],
                isLoading: false,
                durationMs: res.durationMs,
                error: res.error
            });
        }
    }
}
