import React, { useState } from 'react';
import { DashboardLayout } from './layouts/DashboardLayout';
import type { NavTab } from './components/Navbar';
import { HomePage } from './pages/HomePage';
import { BreakItPage } from './pages/BreakItPage';
import { VerifyPage } from './pages/VerifyPage';
import { ComplexityCenterPage } from './pages/ComplexityCenterPage';
import { BenchmarkLabPage } from './pages/BenchmarkLabPage';
import { HistoryReportsPage } from './pages/HistoryReportsPage';

export const App: React.FC = () => {
  const [currentTab, setCurrentTab] = useState<NavTab>('home');

  const renderContent = () => {
    switch (currentTab) {
      case 'home':
        return (
          <HomePage
            onStartInvestigation={() => setCurrentTab('investigate')}
            onOpenVerify={() => setCurrentTab('verify')}
          />
        );
      case 'investigate':
        return <BreakItPage />;
      case 'verify':
        return <VerifyPage />;
      case 'complexity':
        return <ComplexityCenterPage />;
      case 'benchmarks':
        return <BenchmarkLabPage />;
      case 'reports':
        return <HistoryReportsPage />;
      default:
        return (
          <HomePage
            onStartInvestigation={() => setCurrentTab('investigate')}
            onOpenVerify={() => setCurrentTab('verify')}
          />
        );
    }
  };

  return (
    <DashboardLayout currentTab={currentTab} onSelectTab={setCurrentTab}>
      {renderContent()}
    </DashboardLayout>
  );
};

export default App;
