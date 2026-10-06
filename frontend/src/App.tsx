import { BrowserRouter, Routes, Route, Navigate } from 'react-router-dom';
import { Navigation } from './components/Navigation';
import { HomePage } from './pages/HomePage';
import { ExperimentsPage } from './pages/ExperimentsPage';
import { NewExperimentPage } from './pages/NewExperimentPage';
import { ExperimentDetailPage } from './pages/ExperimentDetailPage';
import { ProblemsPage } from './pages/ProblemsPage';
import { DocsPage } from './pages/DocsPage';

export default function App() {
  return (
    <BrowserRouter>
      <div className="app-layout">
        <Navigation />
        <Routes>
          <Route path="/" element={<HomePage />} />
          <Route path="/experiments" element={<ExperimentsPage />} />
          <Route path="/experiments/new" element={<NewExperimentPage />} />
          <Route path="/experiments/:id" element={<ExperimentDetailPage />} />
          <Route path="/problems" element={<ProblemsPage />} />
          <Route path="/docs" element={<DocsPage />} />
          <Route path="*" element={<Navigate to="/" replace />} />
        </Routes>
      </div>
    </BrowserRouter>
  );
}
