export function initResultsList(store) {
    const resultsContainer = document.getElementById('results-container');
    const mainContent = document.getElementById('main-content');

    store.subscribe((state) => {
        if (!state.hasSearched) {
            resultsContainer.classList.remove('active');
            mainContent.classList.remove('has-results');
            return;
        }

        resultsContainer.classList.add('active');
        mainContent.classList.add('has-results');

        if (state.isLoading) {
            renderSkeletons(resultsContainer);
            return;
        }

        if (state.error) {
            renderErrorState(resultsContainer, state.error);
            return;
        }

        if (state.results.length === 0) {
            renderEmptyState(resultsContainer, state.query);
            return;
        }

        renderResults(resultsContainer, state.results);
    });
}

function renderSkeletons(container) {
    let html = '';
    for (let i = 0; i < 4; i++) {
        html += `
            <div class="skeleton-card">
                <div class="skeleton-line url"></div>
                <div class="skeleton-line title"></div>
                <div class="skeleton-line text-1"></div>
                <div class="skeleton-line text-2"></div>
            </div>
        `;
    }
    container.innerHTML = html;
}

function renderErrorState(container, errorMessage) {
    container.innerHTML = `
        <div class="state-box">
            <div class="state-icon"><i class="fa-solid fa-triangle-exclamation" style="color: #ef4444;"></i></div>
            <div class="state-title">Search Connection Error</div>
            <div class="state-desc">${escapeHtml(errorMessage)}</div>
        </div>
    `;
}

function renderEmptyState(container, query) {
    container.innerHTML = `
        <div class="state-box">
            <div class="state-icon"><i class="fa-solid fa-magnifying-glass-minus"></i></div>
            <div class="state-title">No Results Found</div>
            <div class="state-desc">Your search for <strong>"${escapeHtml(query)}"</strong> did not match any indexed documents.</div>
        </div>
    `;
}

function renderResults(container, results) {
    let html = '';
    results.forEach((url, idx) => {
        const formattedUrl = formatDisplayUrl(url);
        const title = generateTitleFromUrl(url);
        const domainLetter = getDomainInitial(url);

        html += `
            <article class="result-card animate-fade-in" style="animation-delay: ${idx * 0.05}s;">
                <div class="result-url-header">
                    <a href="${escapeHtml(url)}" target="_blank" rel="noopener" class="result-url-link">
                        <div class="result-favicon">${domainLetter}</div>
                        <span class="result-url-text">${escapeHtml(formattedUrl)}</span>
                    </a>
                </div>
                <h3 class="result-title">
                    <a href="${escapeHtml(url)}" target="_blank" rel="noopener">${escapeHtml(title)}</a>
                </h3>
                <p class="result-snippet">
                    Indexed web resource matching query terms. Click to view full document contents directly at target URL destination.
                </p>
                <div class="result-actions">
                    <button class="action-btn copy-btn" data-url="${escapeHtml(url)}">
                        <i class="fa-regular fa-copy"></i> Copy URL
                    </button>
                    <a href="${escapeHtml(url)}" target="_blank" rel="noopener" class="action-btn">
                        <i class="fa-solid fa-up-right-from-square"></i> Open Link
                    </a>
                </div>
            </article>
        `;
    });

    container.innerHTML = html;

    // Attach Copy URL button click listeners
    container.querySelectorAll('.copy-btn').forEach(btn => {
        btn.addEventListener('click', (e) => {
            const urlToCopy = btn.getAttribute('data-url');
            navigator.clipboard.writeText(urlToCopy).then(() => {
                btn.innerHTML = `<i class="fa-solid fa-check" style="color: #10b981;"></i> Copied!`;
                setTimeout(() => {
                    btn.innerHTML = `<i class="fa-regular fa-copy"></i> Copy URL`;
                }, 2000);
            });
        });
    });
}

function formatDisplayUrl(url) {
    try {
        const parsed = new URL(url);
        return `${parsed.hostname}${parsed.pathname}`;
    } catch {
        return url;
    }
}

function getDomainInitial(url) {
    try {
        const parsed = new URL(url);
        const host = parsed.hostname.replace(/^www\./, '');
        return host.charAt(0).toUpperCase();
    } catch {
        return 'W';
    }
}

function generateTitleFromUrl(url) {
    try {
        const parsed = new URL(url);
        let path = parsed.pathname.replace(/\/$/, '').split('/').pop();
        if (!path || path === '') {
            return parsed.hostname;
        }
        path = path.replace(/[-_]/g, ' ').replace(/\.[^/.]+$/, '');
        return path.charAt(0).toUpperCase() + path.slice(1);
    } catch {
        return url;
    }
}

function escapeHtml(str) {
    return str.replace(/&/g, '&amp;')
              .replace(/</g, '&lt;')
              .replace(/>/g, '&gt;')
              .replace(/"/g, '&quot;');
}
