// docmd.config.js: basic config for docmd
module.exports = {
  // Core Site Metadata
  siteTitle: 'Documentación del Compilador C-TDS',
  // Define a base URL for your site, crucial for SEO and absolute paths
  // No trailing slash
  siteUrl: 'https://juaniv2002.github.io/compiler/', // Replace with your actual deployed URL
  
  // Logo Configuration
  logo: {
    light: '/assets/images/docmd-logo-light.png', // Path relative to outputDir root
    dark: '/assets/images/docmd-logo-dark.png',   // Path relative to outputDir root
    alt: 'docmd logo',                      // Alt text for the logo
    href: '/',                              // Link for the logo, defaults to site root
  },

  // Directory Configuration
  srcDir: 'documentation',       // Source directory for Markdown files
  outputDir: 'docs',    // Directory for generated static site

  // Sidebar Configuration
  sidebar: {
    collapsible: true,        // or false to disable
    defaultCollapsed: false,  // or true to start collapsed
  },

  // Theme Configuration
  theme: {
    name: 'sky',            // Themes: 'default', 'sky'
    defaultMode: 'light',   // Initial color mode: 'light' or 'dark'
    enableModeToggle: true, // Show UI button to toggle light/dark modes
    positionMode: 'top', // 'top' or 'bottom' for the theme toggle
    codeHighlight: true,    // Enable/disable codeblock highlighting and import of highlight.js
    customCss: [            // Array of paths to custom CSS files
      // '/assets/css/custom.css', // Custom TOC styles
    ]
  },

  // Custom JavaScript Files
  customJs: [  // Array of paths to custom JS files, loaded at end of body
    // '/assets/js/custom-script.js', // Paths relative to outputDir root
    '/assets/js/docmd-image-lightbox.js', // Image lightbox functionality
  ],

  // Content Processing
  autoTitleFromH1: true, // Set to true to automatically use the first H1 as page title
  copyCode: true, // Enable/disable the copy code button on code blocks

  // Plugins Configuration
  // Plugins are configured here. docmd will look for these keys.
  plugins: {
    // SEO Plugin Configuration
    // Most SEO data is pulled from page frontmatter (title, description, image, etc.)
    // These are fallbacks or site-wide settings.
    seo: {
      // Default meta description if a page doesn't have one in its frontmatter
      defaultDescription: 'docmd is a Node.js command-line tool for generating beautiful, lightweight static documentation sites from Markdown files.',
      openGraph: { // For Facebook, LinkedIn, etc.
        // siteName: 'docmd Documentation', // Optional, defaults to config.siteTitle
        // Default image for og:image if not specified in page frontmatter
        // Path relative to outputDir root
        defaultImage: '/assets/images/docmd-preview.png',
      },
      twitter: { // For Twitter Cards
        cardType: 'summary_large_image',     // 'summary', 'summary_large_image'
        // siteUsername: '@docmd_handle',    // Your site's Twitter handle (optional)
        // creatorUsername: '@your_handle',  // Default author handle (optional, can be overridden in frontmatter)
      }
    },
    // Analytics Plugin Configuration
    analytics: {
      // Google Analytics 4 (GA4)
      googleV4: {
        measurementId: 'G-8QVBDQ4KM1' // Replace with your actual GA4 Measurement ID
      }
    },
    // Enable Sitemap plugin
    sitemap: {
      defaultChangefreq: 'weekly',
      defaultPriority: 0.8
    }
    // Add other future plugin configurations here by their key
  },

  // Navigation Structure (Sidebar)
  // Icons are kebab-case names from Lucide Icons (https://lucide.dev/)
  navigation: [
      { title: 'Inicio', path: '/', icon: 'home' },
      {
        title: 'Analizador Sintáctico',
        icon: 'file-code',
        collapsible: true,
        children: [
          { title: 'Descripción General', path: '/parser', icon: 'info' },
          { title: 'Gramática', path: '/parser/grammar', icon: 'git-branch' },
          { title: 'AST', path: '/parser/ast', icon: 'network' },
          { title: 'Ejemplos', path: '/parser/examples', icon: 'book-open' },
        ],
      },
      {
        title: 'Analizador Semántico',
        icon: 'file-check',
        collapsible: true,
        children: [
          { title: 'Descripción General', path: '/semantic-analyzer', icon: 'info' },
          { title: 'Verificaciones', path: '/semantic-analyzer/checks', icon: 'check-circle' },
          { title: 'Tabla de Símbolos', path: '/semantic-analyzer/symbol-table', icon: 'table' },
          { title: 'Ejemplos', path: '/semantic-analyzer/examples', icon: 'book-open' },
        ],
      },
      {
        title: 'Código Intermedio',
        icon: 'code',
        collapsible: true,
        children: [
          { title: 'Descripción General', path: '/codinter', icon: 'info' },
          { title: 'Estructura TAC', path: '/codinter/structure', icon: 'layout' },
          { title: 'Generación', path: '/codinter/generation', icon: 'cpu' },
          { title: 'Ejemplos', path: '/codinter/examples', icon: 'book-open' },
        ],
      },
      {
        title: 'Código Assembly',
        icon: 'cpu',
        collapsible: true,
        children: [
          { title: 'Descripción General', path: '/assembly', icon: 'info' },
          { title: 'Arquitectura x86-64', path: '/assembly/architecture', icon: 'zap' },
          { title: 'Generación de Código', path: '/assembly/generation', icon: 'cog' },
          { title: 'Ejemplos', path: '/assembly/examples', icon: 'book-open' },
        ],
      },
      {
        title: 'Optimizaciones',
        icon: 'sliders',
        collapsible: true,
        children: [
          { title: 'Descripción General', path: '/optimizations', icon: 'info' },
          { title: 'Propagación de Constantes', path: '/optimizations/constant-propagation', icon: 'trending-up' },
          { title: 'Eliminación de Código Muerto', path: '/optimizations/dead-code', icon: 'trash-2' },
          { title: 'Ejemplos y Resultados', path: '/optimizations/examples', icon: 'bar-chart' },
        ],
      },
      
      // External links:
      { title: 'GitHub', path: 'https://github.com/JuaniV2002/compiler', icon: 'github', external: true },
    ],
    
  pageNavigation: true, // Enable previous / next page navigation at the bottom of each page

  // Sponsor Ribbon Configuration
  Sponsor: {
    enabled: false,
    title: 'Support docmd',
    link: 'https://github.com/sponsors/mgks',
  },

  // Footer Configuration
  // Markdown is supported here.
  footer: '© ' + new Date().getFullYear() + ' Project.',

  // Favicon Configuration
  // Path relative to outputDir root
  favicon: '/assets/favicon.ico',
};
