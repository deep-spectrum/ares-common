const applyNewTab = () => {
    const doxy_paths = [
        "doxy-docs/html/"
    ];

    const selector = doxy_paths.map(link => `a[href*="${link}"]`).join(', ');
    const links = document.querySelectorAll(selector);

    links.forEach(link => {
        if (link.getAttribute('target') !== '_blank') {
            link.setAttribute('target', '_blank');
            link.setAttribute('rel', 'noopener noreferrer');
        }
    });
};

applyNewTab();

if (typeof document$ !== "undefined") {
    document$.subscribe(applyNewTab);
}

const observer = new MutationObserver(applyNewTab);
observer.observe(document.body, { childList: true, subtree: true });
